<?php
// $Id$

/**
 * @file
 * Menu callbacks for the SPARQL Views administration UI.
 */

/**
 * Page callback to add a new view.
 */
function sparql_views_add_query() {
  $path = drupal_get_path('module', 'sparql_views');
  drupal_add_css("$path/css/sparql_views_ui.css", 'module');
  $form_state = array(
    'view' => NULL,
    'query' => NULL
  );

  return drupal_build_form('sparql_views_add_form', $form_state);
}

/**
 * Form constructor callback to create the SPARQL Views Add Form, phase 1.
 */
function sparql_views_add_form(&$form_state) {
  $query = $form_state['query'];
  $form = array();

  $form['name'] = array(
    '#type' => 'textfield',
    '#title' => t('View name'),
    '#description' => t('This is the unique name of the view. It must contain only alphanumeric characters and underscores; it is used to identify the view internally and to generate unique theming template names for this view. If overriding a module provided view, the name must not be changed or instead a new view will be created.'),
    '#required' => TRUE,
    '#maxlength' => 32,
    '#default_value' => $view ? $view->name : '',
    '#attributes' => array('dir'=>'ltr'),
  );

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
    '#validate' => array('sparql_views_add_form_validate'),
    '#submit' => array('sparql_views_add_form_submit'),
  );

  return $form;
}

/**
 * Validate the add view form.
 */
function sparql_views_add_form_validate($form, &$form_state) {
  $name = $form_state['values']['name'];

  // View name must be alphanumeric or underscores, no other punctuation.
  if (preg_match('/[^a-zA-Z0-9_]/', $name)) {
    form_error($form['name'], t('View name must be alphanumeric or underscores only.'));
  }

  // View name must not already exist.
  //$view = views_get_view($form_state['values']['name']);
  if ($view && $view->type != t('Default')) {
    form_error($form['name'], t('You must use a unique name for this view.'));
  }
}

/**
 * Process the add view form
 */
function sparql_views_add_form_submit($form, &$form_state) {
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
  drupal_write_record('sparql_views_query', $data);

  views_ui_cache_set($view);
  $form_state['redirect'] ='admin/build/views/edit/' . $view->name;
}
 
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