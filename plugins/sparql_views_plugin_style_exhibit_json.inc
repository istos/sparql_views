<?php
// $Id$

/**
 * @file
 * Views style plugin that outputs an Exhibit JSON feed from a SPARQL Views
 * query.
 */
class sparql_views_plugin_style_exhibit_json extends views_plugin_style_exhibit_json {
  function option_definition() {
    $options = parent::option_definition();
    return $options;
  }

  function options_form(&$form, &$form_state) {
    parent::options_form(&$form, &$form_state);
  }

  function render() {
    if (empty($this->row_plugin)) {
      vpr('sparql_views_plugin_style_exhibit: Missing row plugin');
      return;
    }

    // Setup item types
    $types[$this->view->base_table] = array('label' => $this->options['type_label'],
                                      'pluralLabel' => $this->options['type_plural_label']);

    // Setup item properties
    $props = array();
    foreach ($this->view->field as $id => $field) {
      $props[$id] = array('label' => $field->options['label']);
      
      // @todo Add date and number handling.
      /*
      // Set format for default node fields
      if ($field->options['date_format']) {
        $props[$id]['valueType'] = 'date';
      }
      // Check CCK type fields
      if ($field->content_field && $type = $field->content_field['type']) {
        switch ($type) {
          case 'number_decimal':
          case 'number_float':
          case 'number_integer':
            $props[$id]['valueType'] = 'number';
            break;
          case 'date':
          case 'datetime':
          case 'datestamp':
            $props[$id]['valueType'] = 'date';
        }
      }*/
    }

    // Gather all rendered items
    $items = array();
    foreach ($this->view->result as $row) {
      $item = $this->row_plugin->render($row);
      $items[] = array_merge(array('type' => $this->view->base_table), $item);
    }
    module_load_include('inc', 'exhibit', 'exhibit.pages');
    exhibit_output('application/json', drupal_to_js(exhibit_json(array_values($items), $types, $props)));
    exit;
  }
}