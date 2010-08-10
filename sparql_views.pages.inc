<?php
// $Id$

/**
 * @file
 * Menu callbacks for the SPARQL Views administration UI.
 */

/**
 * Page callback to list SPARQL queries.
 */
function sparql_views_list_queries() {
  $rows = array();

  $query = db_query("SELECT name, data FROM {sparql_views_query}");
  while ($result = db_fetch_array($query)) {
    $query_name = $result['name'];
    $data = unserialize($result['data']);
    $links = array(
      'query' => l('Edit query', 'admin/build/sparql_views/edit/' . $query_name),
      'view' => l('Edit View settings', 'admin/build/views/edit/' . $query_name),
    );
    $rows[] = array(
      $query_name, $data['query'], theme('item_list', $links),
    );
  }

  $output = theme('table', array('Name', 'Query', 'Operations'), $rows);
  dialog_add_js();
  $output .= l('New query', 'admin/build/sparql_views/add/nojs', array('attributes' => array('class' => 'ctools-use-dialog')));
  return $output;
}

function sparql_views_add_query_modal($ajax = FALSE) {
  $path = drupal_get_path('module', 'sparql_views');
  drupal_add_css("$path/css/sparql_views_ui.css", 'module');
  $form_id = 'sparql_views_add_form';
  if ($ajax) {
    ctools_include('ajax');
    $form_state = array(
      'ajax' => TRUE,
      'title' => 'Foo',
    );
    $output = dialog_form_wrapper($form_id, $form_state);
    if (empty($output)) {
      $output[] = ctools_ajax_command_replace('#edit-query-options-query', '<div class="resizable-textarea"><span><textarea cols="60" rows="5" name="query[options][query]" id="edit-query-options-prefixes" class="form-textarea resizable prefix-store textarea-processed">'.$form_state['input']['query'].'</textarea><div class="grippie" style="margin-right: -2px;"></div></span></div>
');
      $output[] = ctools_ajax_command_submit('#views-ui-edit-display-form');
    }
    else {
      foreach ($output as $i => $command) {
        if ($command['command'] == 'dialog_display') {
          $output[$i]['options']['height'] = 900;
          $output[$i]['options']['maxHeight'] = 1000;
          $output[$i]['options']['width'] = 960;

          $output[$i]['options']['position'] = 'center';
          $output[$i]['options']['resizable'] = TRUE;
        }
      }
    }
    ctools_ajax_render($output);
  }
  else {
    return drupal_build_form('sparql_views_add_form', $form_state);
  }
}

/**
 * Page callback to edit SPARQL queries.
 */
function sparql_views_edit_query($view_name) {
  $path = drupal_get_path('module', 'sparql_views');
  drupal_add_css("$path/css/sparql_views_ui.css", 'module');

  $q = db_query("SELECT data FROM {sparql_views_query} WHERE name='$view_name'");
  $result = db_fetch_array($q);
  $data = unserialize($result['data']);

  $form_state = array(
    'view' => views_get_view($view_name),
    'query' => $data['query'],
  );

  return drupal_build_form('sparql_views_edit_form', $form_state);
}

/**
 * Form constructor callback to create the SPARQL Views Add Form.
 */
function sparql_views_add_form(&$form_state) {
  dialog_add_js();
  $query = $form_state['query'];
  $form = array();

  $form['visual_query'] = array(
    '#type' => 'markup',
    '#theme' => 'sparql_views_ui',
    '#title' => t('View description'),
    '#description' => t('This description will appear on the Views administrative UI to tell you what the view is about.'),
    '#default_value' => $view ? $view->description : '',
  );

  $form['query'] = array(
    '#type' => 'textarea',
    '#title' => t('SPARQL query'),
    '#description' => t('The SPARQL query.'),
    '#default_value' => $view ? $view->query : '',
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Next'),
  );
  $form['submit']['#attributes']['class'] = 'ctools-dialog-button';

  return $form;
}

/**
 * Validate the add view form.
 */
function sparql_views_add_form_validate($form, &$form_state) {
}

/**
 * Process the add view form
 */
function sparql_views_add_form_submit($form, &$form_state) {
}

/**
 * Form constructor callback to create the SPARQL Views Add Form.
 */
function sparql_views_edit_form(&$form_state) {
  $view = $form_state['view'];
  $query = $form_state['query'];
  $form = array();

  $query_array = _sparql_views_parse_query($query);
  dpm($query_array);

  $form['visual_query'] = array(
    '#type' => 'markup',
    '#theme' => 'sparql_views_ui',
    '#title' => t('View description'),
    '#description' => t('This description will appear on the Views administrative UI to tell you what the view is about.'),
    '#default_value' => $view ? $view->description : '',
  );

  $form['query'] = array(
    '#type' => 'textarea',
    '#title' => t('SPARQL query'),
    '#description' => t('The SPARQL query.'),
    '#default_value' => $view ? $query : '',
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
    '#validate' => array('sparql_views_edit_form_validate'),
    '#submit' => array('sparql_views_edit_form_submit'),
  );

  return $form;
}

/**
 * Validate the add view form.
 */
function sparql_views_edit_form_validate($form, &$form_state) {

}

/**
 * Process the add view form
 */
function sparql_views_edit_form_submit($form, &$form_state) {
  $view = $form_state['view'] ? $form_state['view'] : views_new_view();
  $view->name = $form_state['values']['name'];
  $view->description = $form_state['values']['description'];
  $view->tag = $form_state['values']['tag'];
  $view->base_table = 'sparql_ep';
  $view->sparql_views['query'] = $form_state['values']['query'];

  // Get the variables (anything that starts with '?' or '$').
  preg_match_all('/(?<=\?).[^\s]*|(?<=\$).[^\s]*/', $view->sparql_views['query'], $matches);
  $view->sparql_views['fields'] = array_unique($matches[0]);

  $data = array(
    'name' => $view->name,
    'description' => $view->description,
    'data' => $view->sparql_views,
  );

  // @todo Check to see if there is already a record and figure out how to
  // handle edits.
  drupal_write_record('sparql_views_query', $data, array('name'));

  views_ui_cache_set($view);
  $form_state['redirect'] ='admin/build/views/edit/' . $view->name;
}



/**
 * Autocomplete callbacks
 */
 
function sparql_views_get_predicates() {
  $output = '';

  //$result = rdfx_sparql_request("SELECT DISTINCT ?p COUNT(?p) AS ?count WHERE {?s ?p ?o} GROUP BY ?p", "http://lab.linkeddata.deri.ie/ppa/sparql.php", array('store_read_key' => 'imp'));
  $result = array(
    'result' => array(
      'rows' => array(
        array('p' => 'foaf:name'),
        array('p' => 'foaf:homepage'),
      )
    )
  );
  
  foreach ($result['result']['rows'] as $row) {
    $output .= "<div id='" . rand(0, 99999999) . "' class='predicate box'>" . $row['p'] . "</div>";
  }
  
  print($output);
}

/**
 * Menu callback; Retrieve a JSON object containing autocomplete suggestions
 * for SPARQL variables.
 */
function sparql_views_variables_autocomplete($string = '') {
  $matches = array();
  $view = views_ui_cache_load(arg(3));
  $display_name = arg(4);

  $fields = _sparql_views_get_fields($view, $display_name);
  if ($string) {
    if ($string == '?') {
      $matches = $fields;
    }
    else {
      foreach ($fields as $field) {
        $string = str_replace('?', '', $string);
        if (strpos(strtolower($field), strtolower($string)) !== FALSE) {
          $matches[$field] = $field;
        }
      }
    }
    // Need to add '?' so when selected, the form value will be a SPARQL
    // variable.
    foreach ($matches as $key => $match) {
      $matches['?' . $key] = $match;
      unset($matches[$key]);
    }
  }
  drupal_json($matches);
}

/**
 * Menu callback; Retrieve a JSON object containing autocomplete suggestions
 * for the predicates used in this endpoint/dataset.
 */
function sparql_views_predicates_autocomplete($string = '') {
  $matches = array();
  // @todo Populate this array with the predicates in the dataset.
  $predicates = array(
    'rdfs:label' => 'rdfs:label',
  );
  foreach ($predicates as $predicate) {
    if (strpos(strtolower($predicate), strtolower($string)) !== FALSE) {
      $matches[$predicate] = $predicate;
    }
  }
  drupal_json($matches);
}

function sparql_views_autocomplete_prefixes($string = '') {
  $matches = array();
  $ns_mappings = _sparql_views_fetch_prefixes();
  foreach ($ns_mappings as $prefix => $namespace) {
    if (strpos(strtolower($prefix), strtolower($string)) !== FALSE) {
      $matches[$prefix] = "PREFIX $prefix: &lt;$namespace&gt;";
    }
  }
  drupal_json($matches);
}