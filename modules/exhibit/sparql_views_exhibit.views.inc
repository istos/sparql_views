<?php
// $Id$

/**
 * SPARQL Views.
 */

/**
 * Implementation of hook_views_plugins().
 */
function sparql_views_exhibit_views_plugins() {
  $path = drupal_get_path('module', 'sparql_views') . '/plugins';
  return array(
    // Style plugins
    'style' => array(
      'sparql_views_exhibit_json' => array(
        'title' => t('Exhibit JSON - SPARQL Views'),
        'help' => t('Generates an Exhibit JSON feed from a SPARQL View.'),
        'handler' => 'sparql_views_plugin_style_exhibit_json',
        'path' => $path,
        'parent' => 'exhibit_json',
        'uses row plugin' => TRUE,
        'uses fields' => TRUE,
        'uses options' => TRUE,
        'type' => 'feed',
      ),
    ),
    
    // Row plugins
    'row' => array(
      'exhibit' => array(
        'title' => t('Fields (Exhibit) - SPARQL Views'),
        'help' => t('Outputs the SPARQL Views fields as an Exhibit JSON object.'),
        'handler' => 'sparql_views_plugin_row_fields_exhibit',
        'path' => $path,
        'uses fields' => TRUE,
        'uses options' => FALSE,
        'type' => 'feed',
        'help topic' => 'style-node-exhibit',
      ),
    ),
  );
}