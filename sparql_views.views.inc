<?php
// $Id$

/**
 * @file
 * Provide Views data, handler and plugins for SPARQL Views. These only
 * depend on SPARQL Views default dependencies. Any handlers or plugins that
 * are dependent on additional modules are placed in their own module in the
 * sparql_views/modules folder.
 */

/**
 * Implements hook_views_data.
 *
 * Declare the base table and all of the fields. Because available fields are
 * not in a known schema but are dynamically defined in the SPARQL query, this
 * requires more complicated logic than usual.
 */
function sparql_views_views_data() {
  $view = NULL;
  $display_name = NULL;

  $data['sparql_ep']['table']['group'] = t('SPARQL');

  $data['sparql_ep']['table']['base'] = array(
    'title' => t('SPARQL query'),
    'help' => t('Connects to a SPARQL endpoint.'),
    'query class' => 'sparql_query',
  );

  $data['sparql_ep']['where'] = array(
    'title' => t('Variable value'),
    'help' => t('Add an equality constraint to a field.'),
    'filter' => array(
      'handler' => 'sparql_views_handler_filter_field_value',
    ),
  );

  // Because views_get_current_view() does not work in this context, we have
  // to figure out the view from the path. This is a dirty hack, if anyone has
  // any better ideas, let me know.
  if (arg(3) == 'ajax' || arg(3) == 'nojs') {
    $view = views_ui_cache_load(arg(5));
    $display_name = arg(6);
  }
  if (arg(3) == 'edit') {
    $view = views_ui_cache_load(arg(4));
    $display_name = NULL;
  }
  $fields = _sparql_views_get_fields($view, $display_name);

  // We dynamically create the fields based on the variables that have been
  // entered into the SPARQL query via filters. This is because RDF datasets
  // do not have set schemas.
  foreach ($fields as $field) {
    $data['sparql_ep'][$field] = array(
      'title' => t($field),
      'help' => t("$field as defined in the SPARQL query."),
      'field' => array(
        'handler' => 'sparql_views_handler_field',
        'click sortable' => TRUE,
      ),
      'sort' => array(
        'handler' => 'views_handler_sort',
      ),
    );
  }

  return $data;
}

/**
 * Implements hook_views_handlers().
 */
function sparql_views_views_handlers() {
  return array(
    'info' => array(
      'path' => drupal_get_path('module', 'sparql_views') .'/handlers',
    ),
    'handlers' => array(
      // Fields
      'sparql_views_handler_field' => array(
        'parent' => 'views_handler_field',
      ),
      // Filters
      'sparql_views_handler_filter_field_value' => array(
        'parent' => 'views_handler_filter_string',
      ),
    ),
  );
}

/**
 * Implements hook_views_plugins().
 */
function sparql_views_views_plugins() {
  $path = drupal_get_path('module', 'sparql_views') .'/plugins';
  return array(
    // Query plugins
    'query' => array(
      'sparql_query' => array(
        'title' => t('SPARQL Query'),
        'help' => t('Query will be generated and run against a SPARQL endpoint.'),
        'handler' => 'sparql_views_plugin_query_sparql',
        //'js' => array(drupal_get_path('module', 'sparql_views') .'/js/sparql_views.js'),
        'path' => $path,
      ),
    ),
  );
}

