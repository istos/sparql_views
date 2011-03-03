<?php
// $Id$

/**
 * @file
 * Views query plugin to build and transmit a SPARQL query to an endpoint.
 */
class sparql_views_plugin_query_sparql extends views_plugin_query {
  var $filter = array();

  /**
   * Constructor; Create the basic query object and fill with default values.
   */
  function init($base_table = 'sparql_ep', $base_field, $query_options) {
    $path = drupal_get_path('module', 'sparql_views');
    // @todo Add JS for hiding irrelvant field items.
    if (module_exists('sparql_views_ui')) {
      dialog_add_js();
      drupal_add_js(array('sparql_views' => array('endpoint' => '')), 'setting');
      drupal_add_js("$path/js/sparql_views.js", 'module');
    }

    $this->endpoint_url = $query_options['endpoint_url'];
    $this->dataset_url = $query_options['dataset_url'];
    $this->endpoint_read_key = $query_options['endpoint_read_key'];
    $this->handwritten_query = $query_options['handwritten_query'];
    $this->prefixes = $query_options['prefixes'];
    $this->select_clause = $query_options['select_clause'];
  }

  function use_pager() {
    return FALSE;
  }

  /**
   * Construct the "WHERE" or "HAVING" part of the query.
   *
   * @param $where
   *   'where' or 'having'.
   */
  function set_condition($query) {
    foreach ($this->filter as $variable_name => $variable) {
      foreach ($variable as $filter) {
        $split_query = preg_split("/(\?$variable_name(.|\n)*?(\.|(?!=})))/", $query, -1, PREG_SPLIT_DELIM_CAPTURE);
        $split_query[1] = $split_query[1] . "\nFILTER $filter";
        $query = implode($split_query);
      }
    }
    return $query;
  }

  /**
   * Generate a query from all of the information supplied to the object.
   */
  function query($view) {
    if ($view->query->handwritten_query == 1) {
      $query = $view->query->select_clause;
    }
    else {
      // This is the same for now.
      $query = $view->query->select_clause;
    }

    $query = $this->set_condition($query);

    return $query;
  }

  /**
   * Get the arguments attached to the WHERE and HAVING clauses of this query.
   */
  function get_where_args() {
    $args = array();
    foreach ($this->where as $group => $where) {
      $args = array_merge($args, $where['args']);
    }

    if ($this->having) {
      foreach ($this->having as $group => $having) {
        $args = array_merge($args, $having['args']);
      }
    }
    return $args;
  }

  /**
   * Create a new grouping for the WHERE or HAVING clause.
   *
   * @param $type
   *   Either 'AND' or 'OR'. All items within this group will be added
   *   to the WHERE clause with this logical operator.
   * @param $group
   *   An ID to use for this group. If unspecified, an ID will be generated.
   * @param $where
   *   'where' or 'having'.
   *
   * @return $group
   *   The group ID generated.
   */
  function set_where_group($type = 'AND', $group = NULL, $where = 'where') {
    // Set an alias.
    $groups = &$this->$where;

    if (!isset($group)) {
      $group = empty($groups) ? 1 : max(array_keys($groups)) + 1;
    }

    if ($type === 'AND') {
      $type = ' ';
    }

    // Create an empty group
    if (empty($groups[$group])) {
      $groups[$group] = array('clauses' => array(), 'args' => array());
    }

    $groups[$group]['type'] = drupal_strtoupper($type);
    return $group;
  }

  function add_filter($type, $variable, $value) {
    $filters = &$this->filter;
    switch ($type) {
      case 'regex':
        $filters[$variable][] = "regex(?$variable, \"$value\")";
    }
  }

  function add_param($param, $value = '') {
    $this->params[$param] = $value;
  }


  function add_argument($variable, $value) {
    dpm($value);
    dpm($variable);
    $search_string = '[' . $variable .']';
    dpm($search_string);
    $this->select_clause = str_replace($search_string, $value, $this->select_clause);
    dpm($this->select_clause);
  }


  function add_where($group, $clause) {
    $args = func_get_args();
    // ditch $group
    array_shift($args);
    // ditch $clause
    array_shift($args);
    // Expand an array of args if it came in.
    if (count($args) == 1 && is_array(reset($args))) {
      $args = current($args);
    }

    // Ensure all variants of 0 are actually 0. Thus '', 0 and NULL are all
    // the default group.
    if (empty($group)) {
      $group = 0;
    }

    // Check for a group.
    if (!isset($this->where[$group])) {
      $this->set_where_group('AND', $group);
    }

    // Add the clause and the args.
    if (is_array($args)) {
      $this->where[$group]['clauses'][] = $clause;
      // we use array_values() here to prevent array_merge errors as keys from multiple
      // sources occasionally collide.
      $this->where[$group]['args'] = array_merge($this->where[$group]['args'], array_values($args));
    }
  }

  /**
   * Let modules modify the query just prior to finalizing it.
   */
  function alter(&$view) {
    foreach (module_implements('sparql_views_query_alter') as $module) {
      $function = $module .'_sparql_views_query_alter';
      $function($view, $this);
    }
  }

  /**
   * Builds the necessary info to execute the query.
   */
  function build(&$view) {
    $this->init_pager($view);
    // @TODO remove as soon as Views alpha4 is released.
    $schema_version = db_result(db_query("SELECT schema_version FROM {system} WHERE name='views'"));
    if ($schema_version >= 6010) {
      if ($this->pager->use_pager()) {
        $this->pager->set_current_page($view->current_page);
      }
    }
    $view->build_info['query'] = $this->query($view);
    $view->build_info['count_query'] = $this->query($view, TRUE);
    $view->build_info['query_args'] = $this->get_where_args();
  }

  /**
   * Executes the query and fills the associated view object with according
   * values.
   *
   * Values to set: $view->result, $view->total_rows, $view->execute_time,
   * $view->pager['current_page'].
   */
  function execute(&$view) {
    $prefixes = $view->query->prefixes;
    $query = $view->build_info['query'];

    if ($query) {
      // Add any basic restrictions set by the filters.
      if (is_array($this->params)) {
        foreach ($this->params as $key => $value) {
          $query = str_replace("?$key", $value, $query);
        }
      }
      // Set the 'LIMIT' clause of the SPARQL query to be the product of
      // the number of pages requested and the number of items per page.
      if ($view->query->pager->options['total_pages'] != 0) {
        $query = $query . ' LIMIT '
          . $view->query->pager->options['total_pages'] * $view->query->pager->options['items_per_page'];
      }

      $start = views_microtime();

      if (!empty($this->endpoint_url)) {
        $endpoint = $this->endpoint_url;
      }
      else {
        $dataset = $this->dataset_url;
      }

      $query = $prefixes .' '. $query;
      if (function_exists('token_replace_multiple')) {
        // We need the array keys from token_get_list, but we need them to have
        // NULL values.
        $token_types = array_fill_keys(array_keys(token_get_list('all')), NULL);
        $query = token_replace_multiple($query, $type = $token_types, $leading = '[', $trailing = ']');
      }
      $results = rdfx_sparql_request($query, $endpoint, array('store_read_key' => $this->endpoint_read_key), array($dataset));
      if ($results['result']) {
        foreach ($results['result']['rows'] as $resource) {
          $view->result[] = $resource;
        }
        // Save the metadata into the object.
        foreach ($results['result']['variables'] as $key => $value) {
          $this->$key = $value;
        }

        $this->execute_time = $this->completed_in;

        // FIXME
        //$this->total_rows = $this->results_per_page * 2;
        //$this->pager['current_page'] = $this->page -1;
      }
    }

    $view->execute_time = views_microtime() - $start;
  }

  // This can only be displayed in the UI if the patch in
  // http://drupal.org/node/621142 is applied.
  function option_definition() {
    $options = parent::option_definition();

    $options['endpoint_url'] = array('default' => '');
    $options['dataset_url'] = array('default' => '');
    $options['endpoint_read_key'] = array('default' => '');
    $options['handwritten_query'] = array('default' => '');
    $options['prefixes'] = array('default' => '');
    $options['select_clause'] = array('default' => '');

    return $options;
  }

  function admin_summary() {
    if (!empty($this->options['exposed']))
      {}
  }

  function options_form(&$form, &$form_state) {
    $form['endpoint_url'] = array(
      '#type' => 'textfield',
      '#title' => t('SPARQL Endpoint'),
      '#default_value' => $this->endpoint_url,
      '#description' => t("The SPARQL endpoint that you want to access. For instance, !dbpedia.",
        array('!dbpedia' => l('http://dbpedia.org/sparql', 'http://dbpedia.org/sparql'))
      ),
    );
    $form['dataset_url'] = array(
      '#type' => 'textfield',
      '#title' => t('RDFa Dataset'),
      '#default_value' => $this->dataset_url,
      '#description' => t("The SPARQL endpoint that you want to access. For instance, !dbpedia.",
        array('!dbpedia' => l('http://dbpedia.org/sparql', 'http://dbpedia.org/sparql'))
      ),
    );
    $form['endpoint_read_key'] = array(
      '#type' => 'textfield',
      '#title' => t('Read Key'),
      '#default_value' => $this->endpoint_read_key,
      '#description' => t("Key for read access to the endpoint."),
    );
    $form['visual_query'] = array();
    $form['handwritten_query'] = array(
      '#type' => 'checkbox',
      '#title' => t('Write your own SPARQL query'),
      '#default_value' => $this->handwritten_query,
    );
    $form['prefixes'] = array(
      '#type' => 'textarea',
      '#title' => t('Prefixes'),
      // Have to encode the prefix value when using it with SPARQL Views UI.
      '#default_value' => $this->prefixes,
      '#description' => t("Prefixes to use for this query."),
      '#attributes' => array('class' => 'prefix-store'),
      '#process' => array('views_process_dependency'),
      '#dependency' => array(
        'edit-query-options-handwritten-query' => array(1),
      ),
    );
    $form['select_clause'] = array(
      '#type' => 'textarea',
      '#title' => t('Select query'),
      '#default_value' => $this->select_clause,
      '#description' => t("Prefixes to use for this query."),
      '#process' => array('views_process_dependency'),
      '#dependency' => array(
        'edit-query-options-handwritten-query' => array(1),
      ),
    );

    if (module_exists('sparql_views_ui')) {
      $form['visual_query'] = array(
        '#type' => 'markup',
        '#value' => l(t('Build graph'), 'admin/build/sparql_views/add/nojs', array('attributes' => array('class' => 'ctools-use-dialog'))),
      );
    }
    else {
      $form['visual_query'] = array(
        '#type' => 'markup',
        '#value' => t("<strong>New to SPARQL?</strong> A visual SPARQL query builder is available when you enable SPARQL Views UI on the !modules page.", array('!modules' => l('module administration', 'admin/build/modules'))),
      );
    }
  }

  function options_submit(&$form, &$form_state) {
    $form_state['values']['query']['options']['prefixes'] = urldecode($form_state['input']['query']['options']['prefixes']);
  }
}

