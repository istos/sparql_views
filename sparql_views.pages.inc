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
  $predicates = array(
    'rdfs:label' => 'rdfs:label',
    'http://purl.org/dc/terms/creator' => 'http://purl.org/dc/terms/creator',
    'rdfs:type' => 'rdfs:type',
    'foaf:interest' => 'foaf:interest',
    'http://purl.org/dc/terms/title' => 'http://purl.org/dc/terms/title',
    'foaf:homepage' => 'foaf:homepage', 
    'foaf:openid' =>'foaf:openid', 
    'foaf:mbox' => 'foaf:mbox',
    'foaf:depiction' => 'foaf:depiction',
    'foaf:name' => 'foaf:name',
    'foaf:knows' => 'foaf:knows',
    'foaf:based_near' => 'foaf:based_near',
    'http://www.w3.org/2003/01/geo/wgs84_pos#lat' => 'http://www.w3.org/2003/01/geo/wgs84_pos#lat',
    'http://www.w3.org/2003/01/geo/wgs84_pos#long' => 'http://www.w3.org/2003/01/geo/wgs84_pos#long',
    'http://www.w3.org/2002/07/owl#sameAs' => 'http://www.w3.org/2002/07/owl#sameAs',
    'http://open.vocab.org/terms/businessCard' => 'http://open.vocab.org/terms/businessCard',
    'http://www.w3.org/2006/vcard/ns#org' => 'http://www.w3.org/2006/vcard/ns#org',
    'http://www.w3.org/2006/vcard/ns#organization-name' => 'http://www.w3.org/2006/vcard/ns#organization-name',
    'http://www.w3.org/2006/vcard/ns#workAdr' => 'http://www.w3.org/2006/vcard/ns#workAdr',
    'http://www.w3.org/2006/vcard/ns#country-name' => 'http://www.w3.org/2006/vcard/ns#country-name',
    'http://www.w3.org/2006/vcard/ns#locality' => 'http://www.w3.org/2006/vcard/ns#locality',
    'http://www.w3.org/2006/vcard/ns#street-address' => 'http://www.w3.org/2006/vcard/ns#street-address',
    'http://www.w3.org/2006/vcard/ns#workTel' => 'http://www.w3.org/2006/vcard/ns#workTel',
    'http://rdf.data-vocabulary.org/rdf.xml#nickname' => 'http://rdf.data-vocabulary.org/rdf.xml#nickname',
    'http://www.w3.org/2000/01/rdf-schema#seeAlso' => 'http://www.w3.org/2000/01/rdf-schema#seeAlso',
    'foaf:weblog' => 'foaf:weblog',
    'http://rdf.data-vocabulary.org/rdf.xml#name' => 'http://rdf.data-vocabulary.org/rdf.xml#name',
    'foaf:workplaceHomepage' => 'foaf:workplaceHomepage',
    'foaf:nick' => 'foaf:nick',
    'foaf:primaryTopic' => 'foaf:primaryTopic',
    'http://www.w3.org/ns/auth/rsa#modulus' => 'http://www.w3.org/ns/auth/rsa#modulus',
    'http://www.w3.org/ns/auth/cert#hex' => 'http://www.w3.org/ns/auth/cert#hex',
    'http://www.w3.org/ns/auth/cert#identity' => 'http://www.w3.org/ns/auth/cert#identity',
    'http://www.w3.org/ns/auth/rsa#public_exponent' => 'http://www.w3.org/ns/auth/rsa#public_exponent',
    'http://www.w3.org/ns/auth/cert#decimal' => 'http://www.w3.org/ns/auth/cert#decimal',
    'foaf:currentProject' => 'foaf:currentProject',
    'mpp:performedBy' => 'mpp:performedBy',
    'mpp:inVersion' => 'mpp:inVersion',
    'mpp:supportsInput' => 'mpp:supportsInput',
    'mpp:supportsOutput' => 'mpp:supportsOutput',
    'mpp:suitableFor' => 'mpp:suitableFor',
  );
  foreach ($predicates as $predicate) {
    if (strpos(strtolower($predicate), strtolower($string)) !== FALSE) {
      $matches[$predicate] = $predicate;
    }
  }
  drupal_json($matches);
}