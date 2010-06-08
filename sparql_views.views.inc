<?php
// $Id$

/**
 * SPARQL Views.
 */

function sparql_views_views_data() {
  $fields = _sparql_views_get_fields();
  
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
      'sparql_views_handler_field_link' => array(
        'parent' => 'sparql_views_handler_field',
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
        'path' => $path,
      ),
    ),
  );
}

function _sparql_views_get_fields() {
  $fields = array();
  $sparql_view_names = array();
  
  if (arg(4) == 'add-item') {
    $view = views_ui_cache_load(arg(5));
    foreach($view->display[arg(6)]->display_options['filters'] as $filter) {
      foreach ($filter['value'] as $value) {
        if (strpos($value, '?') === 0) {
          $field = substr($value, 1);
          $fields[$field] = $field;
        }
      }
    }
  }
  
  //@todo Move this to _sparql_views_get_all_fields which could be Drupal static.
  else {
    $views = views_get_all_views();
    foreach ($views as $view_name => $view) {
      $query_type = $view->display['default']->display_options['query']['type'];
      if ($query_type == 'sparql_query') {
        $sparql_view_names[] = $view_name;
      }
    }
    foreach ($sparql_view_names as $view_name) {
      // Get the view the user is editing based on the URL.
      $view = views_ui_cache_load($view_name);
      
      foreach ($view->display as $display) {
        //if (isset($display->display_options['filters'])) {
          foreach ($display->display_options['filters'] as $filter) {
            foreach ($filter['value'] as $value) {
              if (strpos($value, '?') === 0) {
                $field = substr($value, 1);
                $fields[$field] = $field;
              }
            }
          }
        //}
      }
    }
  }
  
  return $fields;
}
