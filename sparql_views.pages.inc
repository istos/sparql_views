<?php
// $Id$

/**
 * @file
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
        if (strpos(drupal_strtolower($field), drupal_strtolower($string)) !== FALSE) {
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
    if (strpos(drupal_strtolower($predicate), drupal_strtolower($string)) !== FALSE) {
      $matches[$predicate] = $predicate;
    }
  }
  drupal_json($matches);
}

function sparql_views_autocomplete_prefixes($string = '') {
  $matches = array();
  $ns_mappings = _sparql_views_get_prefixes();
  foreach ($ns_mappings as $prefix => $namespace) {
    if (strpos(drupal_strtolower($prefix), drupal_strtolower($string)) !== FALSE) {
      $matches[$prefix] = "PREFIX $prefix: &lt;$namespace&gt;";
    }
  }
  drupal_json($matches);
}