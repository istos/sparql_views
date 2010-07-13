<?php
// $Id$

/**
 * @file
 * Views row plugin to render SPARQL Views result row.
 */
class sparql_views_plugin_row_fields_exhibit extends views_plugin_row {
  function option_definition() {
    $options = parent::option_definition();
    $options['item_label'] = array('default' => $this->view->base_table, 'translatable' => TRUE);
    return $options;
  }

  function options_form(&$form, &$form_state) {
    $options = parent::option_definition();
    $view_name = arg(5);
    $display_name = arg(6);

    $form['item_label'] = array(
      '#type' => 'textfield',
      '#title' => t('Title field'),
      '#autocomplete_path' => "sparql_views/autocomplete/subject-object/$view_name/$display_name",
      '#default_value' => $this->options['item_label'],
      '#description' => t('This is the field to use as the title of the item.'),
    );
  }

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
      $item_label = str_replace('?', '', $this->options['item_label']);
      $output['label'] = $row[$item_label][0]['field'];
    }

    return $output;
  }
}

