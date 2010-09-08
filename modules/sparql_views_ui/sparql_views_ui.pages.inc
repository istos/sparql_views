<?php
// $Id$

/**
 * @file
 * Menu callbacks for the SPARQL Views administration UI.
 */
function sparql_views_ui_add_query_modal($ajax = FALSE) {
  $path = drupal_get_path('module', 'sparql_views');
  drupal_add_css("$path/css/sparql_views_ui.css", 'module');
  $form_id = 'sparql_views_ui_add_form';
  if ($ajax) {
    ctools_include('ajax');
    $form_state = array(
      'ajax' => TRUE,
      'title' => 'SPARQL Query Builder',
    );
    $output = dialog_form_wrapper($form_id, $form_state);
    if (empty($output)) {
      // @todo Fix this hack with a innerHTML replace in CTools.
      $output[] = ctools_ajax_command_replace('#edit-query-options-prefixes', '<div class="resizable-textarea"><span><textarea cols="60" rows="5" name="query[options][prefixes]" class="form-textarea resizable textarea-processed">'.$form_state['input']['prefixes'].'</textarea><div class="grippie" style="margin-right: -2px;"></div></span></div>
');
      $output[] = ctools_ajax_command_replace('#edit-query-options-select-clause', '<div class="resizable-textarea"><span><textarea cols="60" rows="5" name="query[options][select_clause]" class="form-textarea resizable textarea-processed">'.$form_state['input']['select_clause'].'</textarea><div class="grippie" style="margin-right: -2px;"></div></span></div>
');
      $output[] = ctools_ajax_command_submit('#views-ui-edit-display-form');
    }
    else {
      foreach ($output as $i => $command) {
        if ($command['command'] == 'dialog_display') {
          $output[$i]['options']['height'] = 700;
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
 * Form constructor callback to create the SPARQL Views Add Form.
 */
function sparql_views_ui_add_form(&$form_state) {
  dialog_add_js();
  $form = array();

  $form['visual_query'] = array(
    '#type' => 'markup',
    '#theme' => 'sparql_views_ui',
    '#title' => t('View description'),
    '#description' => t('This description will appear on the Views administrative UI to tell you what the view is about.'),
  );

  $form['select_clause'] = array(
    '#type' => 'textarea',
    '#title' => t('SPARQL query'),
  );

  $form['prefixes'] = array(
    '#type' => 'textarea',
    '#title' => t('Prefixes'),
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save settings'),
  );
  $form['submit']['#attributes']['class'] = 'ctools-dialog-button';

  return $form;
}

/**
 * Validate the add view form.
 */
function sparql_views_ui_add_form_validate($form, &$form_state) {
}

/**
 * Process the add view form
 */
function sparql_views_ui_add_form_submit($form, &$form_state) {
}

/**
 * Autocomplete callbacks
 */
 
function sparql_views_ui_get_predicates() {
  // @todo Cache this.
  $output = '';
  $prefixes = array();
  $query = "SELECT DISTINCT ?p WHERE {?s ?p ?o} ORDER BY ?p LIMIT 10 OFFSET " . $_POST['recurssionCount']*10;
  $result = rdfx_sparql_request($query, $_POST['endpoint'], array('store_read_key' => $_POST['storeReadKey']));
  if (empty($result)) {
    print('Sorry, this has timed out.');
  }
  if (empty($result['result']['rows'])) {
    print('done');
    exit;
  }
  else {
    foreach ($result['result']['rows'] as $row) {
      $term = $row['p'];
      // Check whether there is a matching NS mapping on prefix.cc.
      $ns_mappings = _sparql_views_get_prefixes();
      preg_match('/.*\#|.*\//', $term, $matches);
      $test_uri = $matches[0];
      if (isset($ns_mappings[$test_uri])) {
        $term = str_replace($test_uri, $ns_mappings[$test_uri] . ":", $row['p']);
      }
      else {
        //$term = "&lt;$term&gt;";
      }
      $output .= "<div id='" . rand(0, 99999999) . "' class='predicate box'>" . $term . "</div>";
    }
    print($output);
  }
}

function sparql_views_ui_get_prefix_declaration($prefixes = array()) {
  $used_ns = array();
  $ns_mappings = _sparql_views_get_prefixes();
  foreach ($prefixes as $prefix) {
    if (isset($ns_mappings[$prefix])) {
      $used_ns[$prefix] = $ns_mappings[$prefix];
    }
  }
  $output = theme('prefix_declaration', $used_ns);
  return $output;
}

function sparql_views_ui_print_prefix_declaration() {
  if (empty($_POST['prefixes'])) {
    exit;
  }
  else {
    $output = sparql_views_ui_get_prefix_declaration($_POST['prefixes']);
    print($output);
  }
}

function sparql_views_ui_get_result_preview() {
  $table_rows = array();
  $header = array();
  $query = $_POST['query'];
  $result = rdfx_sparql_request($query, $_POST['endpoint'], array('store_read_key' => $_POST['storeReadKey']));
  if (isset($result['result'])) {
    $variables = $result['result']['variables'];
    $header = $variables;
    foreach ($result['result']['rows'] as $row) {
      $table_row = array();
      foreach ($variables as $variable){
        $table_row[] = ($row[$variable]) ? $row[$variable] : '-';
      }
      $table_rows[] = $table_row;
    }
  }
  $output = theme('table', $header, $table_rows);
  print $output;
}