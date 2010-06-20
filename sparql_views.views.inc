<?php
// $Id$

/**
 * SPARQL Views.
 */

function sparql_views_views_data() {
  $data['sparql_ep']['table']['group']  = t('SPARQL');

  $data['sparql_ep']['table']['base'] = array(
    'title' => t('SPARQL query'),
    'help' => t('Connects to a SPARQL endpoint.'),
    'query class' => 'sparql_query'
  );

  $data['sparql_ep']['where'] = array(
    'title' => t('Where'),
    'help' => t('Add a triple pattern where clause.'),
    'filter' => array(
      'handler' => 'sparql_views_handler_filter_where',
    ),
  );
  
  if (arg(4) == 'add-item' || arg(4) == 'config-item') {
    $view = views_ui_cache_load(arg(5));
    $display_name = arg(6);
  }
  $fields = _sparql_views_get_fields($view, $display_name);
  
  foreach ($fields as $field) {
    $data['sparql_ep'][$field] = array(
      'title' => t($field),
      // Shouldn't there be sparql_views_handler_field_markup??
      'help' => t("$field as defined in the SPARQL:Where filter."),
      'field' => array(
        'handler' => 'sparql_views_handler_field_markup',
      ),
    );
  }

  return $data;
}

/**
 * Implementation of hook_views_handlers().
 */
function sparql_views_views_handlers() {
  return array(
    'info' => array(
      'path' => drupal_get_path('module', 'sparql_views') . '/handlers',
    ),
    'handlers' => array(
      // Fields
      'sparql_views_handler_field' => array(
        'parent' => 'views_handler_field'
      ),
      'sparql_views_handler_field_markup' => array(
        'parent' => 'sparql_views_handler_field',
      ),

      // Filters
      'sparql_views_handler_filter_where' => array(
        'parent' => 'sparql_views_handler_filter'
      ),
      
      'sparql_views_handler_filter' => array(
        'parent' => 'views_handler_filter'
      ),
    ),
  );
}

/**
 * Implementation of hook_views_plugins().
 */
function sparql_views_views_plugins() {
  $path = drupal_get_path('module', 'sparql_views') . '/plugins';
  return array(
    'query' => array(
      'sparql_query' => array(
        'title' => t('SPARQL Query'),
        'help' => t('Query will be generated and run against a SPARQL endpoint.'),
        'handler' => 'sparql_views_plugin_query_sparql',
        'js' => array(drupal_get_path('module', 'sparql_views') .'/js/sparql_views.js'),
        'path' => $path,
      ),
    ),
  );
}