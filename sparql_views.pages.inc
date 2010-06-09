<?php
// $Id$

/**
 * @file
 * SPARQL Views callback file.
 */

/**
 * Menu callback; Retrieve a JSON object containing autocomplete suggestions
 * for SPARQL variables.
 */
function sparql_views_variables_autocomplete($string = '') {
  $matches = array();
  $fields = _sparql_views_get_fields();
  if ($string) {
    if ($string == '?') {
      $matches = $fields;
    }
    else {
      //$string = str_replace('?', '', $string);
      foreach ($fields as $field) {
        if (strpos($field, $string) !== FALSE) {
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