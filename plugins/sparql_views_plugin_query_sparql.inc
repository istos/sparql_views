<?php
// $Id$

/**
 * @file
 * Views query plugin to build and transmit a SPARQL query to an endpoint.
 */

class sparql_views_plugin_query_sparql extends views_plugin_query {

  /**
   * Constructor; Create the basic query object and fill with default values.
   */
  function init($base_table = 'sparql_ep', $base_field, $query_options) {
    // @todo Add JS for hiding irrelvant field items.

    $this->endpoint_url = $query_options['endpoint_url'];
    $this->endpoint_read_key = $query_options['endpoint_read_key'];
    $this->prefixes = $query_options['prefixes'];
  }

  /**
   * Construct the "WHERE" or "HAVING" part of the query.
   *
   * @param $where
   *   'where' or 'having'.
   */
  function condition_query($where = 'where') {
    $clauses = array();
    if ($this->$where) {
      foreach ($this->$where as $group => $info) {
        $clause = implode($info['type'], $info['clauses']);
        if (count($info['clauses']) > 1) {
          $clause = $clause;
        }
        $clauses[] = $clause;
      }

      // Unset the empty clause that is added (apparently by default).
      unset($clauses[0]);
      if ($clauses) {
        if (count($clauses) > 1) {
          return implode(")". $this->group_operator, $clauses);
        }
        else {
          return array_shift($clauses);
        }
      }
    }
    return "";
  }

  function use_pager() {
    return FALSE;
  }

  /**
   * Generate a query and a countquery from all of the information supplied
   * to the object.
   *
   * @param $get_count
   *   Provide a countquery if this is true, otherwise provide a normal query.
   */
  function query($view, $get_count = FALSE) {

    $where = $this->condition_query();

    $params = $this->params;
    if (!empty($view->pager['items_per_page'])) {
      $params['rpp'] = $view->pager['items_per_page'] <= 100 ? $view->pager['items_per_page'] : 100;
    }
    $params['WHERE'] = $where;

    // Build the full string.
    $items = array();
    if ($params) {
      foreach ($params as $key => $value) {
        if ($key == 'q') {
          array_unshift($items, 'query='. urlencode($value));
        }
        else if ($key == 'WHERE') {
          $where_condition = "{". $value ."}";
          $items[] = $key . $where_condition;
        }
        else {
          $items[] = $key .'='. urlencode($value);
        }
      }
    }
    $params = implode('&', $items);

    return $params;
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

    $groups[$group]['type'] = strtoupper($type);
    return $group;
  }

  function add_param($param, $value = '') {
    $this->params[$param] = $value;
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
    if ($this->pager->use_pager()) {
      $this->pager->set_current_page($view->current_page);
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
    $query = $view->build_info['query'];

    if ($query) {
      $replacements = module_invoke_all('views_query_substitutions', $view);
      $query = str_replace(array_keys($replacements), $replacements, $query);

      $start = views_microtime();

      $endpoint = $this->endpoint_url;
      // @todo fix this after demo to replace all args, not just one
      /*foreach ($view->args as $arg_num) {
        $query = str_replace('%1', urldecode($view->args[0]), $query);
      }*/
      $query_test = array('type' => 'property',
        'query' => "$this->prefixes SELECT * $query",
      );
      $results = rdfx_sparql_request($query_test['query'], $endpoint, array('store_read_key' => $this->endpoint_read_key));
      if ($results['result']) {
        foreach ($results['result']['rows'] as $resource) {
          $view->result[] = $resource;
        }
        // Save the metadata into the object.
        unset($results['result']);
        foreach ($results as $key => $value) {
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

  function add_signature(&$view) {
    //$view->query->add_field(NULL, "'" . $view->name . ':' . $view->current_display . "'", 'view_name');
  }

  // This can only be displayed in the UI if the patch in
  // http://drupal.org/node/621142 is applied.
  function option_definition() {
    $options = parent::option_definition();
    return $options;
  }

  function admin_summary() {
    if (!empty($this->options['exposed'])) {}
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
    $form['load_predicates'] = array(
      '#type' => 'checkbox',
      '#title' => t('Load predicates used in dataset'),
      '#default_value' => TRUE,
      '#description' => t("You can use autocompltion for predicates in your query if you load the predicates used in the dataset. Note, loading predicates will take a while."),
    );
    $form['endpoint_read_key'] = array(
      '#type' => 'textfield',
      '#title' => t('Read Key'),
      '#default_value' => $this->endpoint_read_key,
      '#description' => t("Key for read access to the endpoint."),
    );
    $form['prefixes'] = array(
      '#type' => 'textarea',
      '#title' => t('Prefixes'),
      '#default_value' => $this->prefixes,
      '#description' => t("Prefixes to use for this query."),
      '#attributes' => array('class' => 'prefix-store'),
    );
    $form['prefixes_testing'] = array(
      '#type' => 'textfield',
      '#title' => t('Prefixes'),
      '#autocomplete_path' => 'sparql_views/prefixes/autocomplete',
      '#description' => t("Prefixes to use for this query."),
    );
  }

  function options_validate(&$form, &$form_state) {}

  function options_submit(&$form, &$form_state) {
    $endpoint = $form_state['values']['query']['options']['endpoint_url'];
    $access_key = $form_state['values']['query']['options']['endpoint_read_key'];

    // If the user wants to load the predicates, do a SPARQL request and cache
    // the predicates.
    //if ($form_state['values']['query']['options']['load_predicates'] === TRUE) {
    $results = rdfx_sparql_request("SELECT DISTINCT ?p WHERE {?s ?p ?o}", $endpoint, array('store_read_key' => $access_key));
    foreach ($results['result']['rows'] as $result) {
      $predicates[] = $result['p'];
    }
    //cache_set();
    //}
  }
}

