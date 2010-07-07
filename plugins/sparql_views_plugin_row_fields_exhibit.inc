<?php
// $Id: views_plugin_row_fields_exhibit.inc,v 1.4 2009/05/01 05:09:26 arto Exp $

//////////////////////////////////////////////////////////////////////////////
// Views API plugins

/**
 * Views row plugin that invokes node_view() on each node and prepares its
 * body for inclusion as an RSS 1.0 item description in the feed.
 *
 * @see http://web.resource.org/rss/1.0/spec
 * @ingroup views_row_plugins
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

