<?php
// $Id$

/**
 * @file
 * Views row plugin to render SPARQL Views result row.
 */
class sparql_views_plugin_row_fields_exhibit extends views_plugin_row {
  function render(&$row) {
    $output = array();

    // Run through and render each field individually instead of using
    // advanced_render because advanced_render returns multivalue fields as
    // one string instead of as an array. This may be a bug in advanced_render.
    foreach ($this->view->field as $id => $field) {
      $raw_items = $field->get_items($row);
      
      foreach ($raw_items as $count => $item) {
        // Override here.
        $rendered = $field->render_item($count, $item);
        $field->last_render = $rendered['field'];
        $field->original_value = $this->last_render;

        $alter = $item + $field->options['alter'];
        if ($field->options['exclude'] == FALSE) {
          $items[$id][] = $field->render_text($alter);
        }
      }
      $output[$id] = $items[$id];

      $output['label'] = $row['pub_title'][0]['field'];
    }

    return $output;
  }
}

