<?php
// $Id$

/**
 * @file
 * Autocomplete callbacks for the SPARQL Views administration UI.
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