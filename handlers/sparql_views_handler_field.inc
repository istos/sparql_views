<?php
// $Id$

/**
 * @file
 * Views field handler that renders SPARQL query results as returned from ARC.
 */

class sparql_views_handler_field extends views_handler_field {

  /**
   * Add SPARQL query specific field options.
   */
  function option_definition() {
    $options = parent::option_definition();

    $options['alter']['contains']['combine_values'] = array('default' => FALSE);
    $options['alter']['contains']['separator'] = array('default' => FALSE);

    return $options;
  }

  /**
   * Add form elements for SPARQL query specific field options.
   */
  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);
    $form['alter']['combine_values'] = array(
      '#type' => 'checkbox',
      '#title' => t('Combine multiple values'),
      '#description' => t('If checked, rows that contain all the same values except for this field will be combined and this field will become a multivalue field.'),
      '#default_value' => $this->options['alter']['combine_values'],
    );
    $form['alter']['separator'] = array(
      '#title' => t('Separator'),
      '#type' => 'textfield',
      '#default_value' => $this->options['alter']['separator'],
      '#description' => t('The separator between multiple values in a combined field.'),
      '#process' => array('views_process_dependency'),
      '#dependency' => array(
        'edit-options-alter-combine-values' => array(1),
      ),
      '#maxlength' => 12,
    );
  }

  /**
   * Called to add the field to a query.
   */
  function query() {
    $this->field_alias = $this->real_field;
  }

  /**
   * Override pre_render in views_handler_field.
   *
   * This override is required to create the array structure needed to combine
   * multivalue fields.
   */
  function pre_render(&$values) {
    static $sparql_views_storage;

    // If this is the first time through the function pre_render, create an
    // array representing the rows to combine.
    reset($this->view->field);
    $first_key = key($this->view->field);
    if ($this->field_alias == $first_key && count($values) > 1) {
      $sparql_views_storage['combine'] = $this->calculate_row_combinations($values);
    }

    // For each field, iterate through and combine values as defined in
    // $sparql_views_storage['combine'] array.
    foreach ($values as $key => $value) {
      $items = array();
      $field = $value[$this->field_alias];

      // Copy the untampered field value into a storage array.
      if (!isset($sparql_views_storage['field_values'][$this->field_alias][$key])) {
        $sparql_views_storage['field_values'][$this->field_alias][$key] = $field;
      }
      $token = $this->options['alter']['path'];
      // If this field is linked to another field, then add the token in a path.
      if (!empty($token)) {
        $token_fieldname = preg_replace('/\[|\]/', '', $token);
        $link = isset($sparql_views_storage['field_values'][$token_fieldname][$key]) ? $sparql_views_storage['field_values'][$token_fieldname][$key] : $value[$token_fieldname];
      }
      if (!empty($field)) {
        $items[] = array('field' => $field, 'token' => array($token => $link));
      }

      if (isset($sparql_views_storage['combine'][$key][$this->field_alias])) {
        $rows_to_combine = $sparql_views_storage['combine'][$key][$this->field_alias];
        foreach ($rows_to_combine as $row) {
          if (!isset($sparql_views_storage['field_values'][$this->field_alias][$row])) {
            $field = $values[$row][$this->field_alias];
            $sparql_views_storage['field_values'][$this->field_alias][$row] = $field;
          }
          $token = $this->options['alter']['path'];
          if (!empty($token)) {
            $token_fieldname = preg_replace('/\[|\]/', '', $token);
            $link = isset($sparql_views_storage['field_values'][$token_fieldname][$row]) ? $sparql_views_storage['field_values'][$token_fieldname][$row] : $values[$row][$token_fieldname];
          }
          if (!empty($field)) {
            $items[] = array('field' => $field, 'token' => array($token => $link));
          }
          $values[$row]['rendered'] = TRUE;
          unset($values[$row]);
        }
      }
      $values[$key][$this->field_alias] = $items;
    }
  }

  /**
   * Override get_items in view.inc.
   *
   * This override is required because the array structure created by
   * pre_render needs to be parsed out on a row by row basis.
   */
  function get_items($values, $display_id = NULL) {
    $items = $values[$this->field_alias];
    return $items;
  }

  /**
   * This function is required in order for advanced_render to use get_items.
   * Function advanced_render could be rewritten to make this unnecessary, but
   * we prefer to keep advanced_render as close to its parent function as
   * possible.
   *
   * This is modelled on views_handler_field_prerender_list.
   */
  function render_item($count, $item) {
    return $item;
  }

  /**
   * This function is required in order for advanced_render to combine the items
   * returned by render_item.
   *
   * This is modelled on views_handler_field_prerender_list.
   */
  function render_items($items) {
    if (!empty($items) && $this->options['alter']['combine_values'] == TRUE) {
      if ($this->options['alter']['separator'] != FALSE) {
        return implode(check_plain($this->options['alter']['separator']), $items);
      }
      else {
        return theme('item_list', $items, NULL, $this->options['type']);
      }
    }
    else {
      return check_markup($items[0]);
    }
  }

  /**
   * Override advanced_render in views_handler_field.
   *
   * This override is required to parse the array structure that we created in
   * pre_render to handle multivalue fields.
   */
  function advanced_render($values) {
    if ($this->allow_advanced_render() && method_exists($this, 'render_item')) {
      $raw_items = $this->get_items($values);
    }
    else {
      $this->last_render = $value = $this->render($values);
      $this->original_value = $value;
    }

    if ($this->allow_advanced_render()) {
      $tokens = NULL;
      if (method_exists($this, 'render_item')) {
        $items = array();
        foreach ($raw_items as $count => $item) {
          // Override here.
          $rendered = $this->render_item($count, $item);
          $this->last_render = $rendered['field'];
          $this->original_value = $this->last_render;

          $alter = $item + $this->options['alter'];
          $items[] = $this->render_text($alter);
        }

        $value = $this->render_items($items);
        if (!empty($alter['strip_tags'])) {
          $value = strip_tags($value);
        }
      }
      else {
        $value = $this->render_text($this->options['alter']);
      }

      // This happens here so that render_as_link can get the unaltered value of
      // this field as a token rather than the altered value.
      $this->last_render = $value;
    }

    if (empty($this->last_render)) {
      if (($this->last_render !== 0 && $this->last_render !== '0') || !empty($this->options['empty_zero'])) {
        $this->last_render = $this->options['empty'];
      }
    }

    return trim($this->last_render);
  }

  /**
   * Override get_render_tokens in views_handler_field.
   *
   * This adds the token that is stored with the item itself. We need this
   * to keep the association between a row value and its token when we combine.
   */
  function get_render_tokens($item) {
    $tokens = parent::get_render_tokens($item);
    foreach ($this->view->display_handler->get_handlers('field') as $field => $handler) {
      $tokens = array_merge($tokens, $item['token']);
    }
    return $tokens;
  }

  /****************************************************************************
   * Helper functions.
   */
  protected function calculate_row_combinations($values) {
    $combine_rows = array();

    // Get a list of fields that are linked to other fields if the other
    // field is excluded from display. We do this because we can combine two
    // rows if two field values are different ONLY IF the two are connected
    // in this way. Otherwise, we only combine rows if one value is different.
    $linked_fields = array();
    foreach ($this->view->field as $field) {
      $link_path = preg_replace('/\[|\]/', '', $field->options['alter']['path']);
      if (!empty($link_path) && $this->view->field[$link_path]->options['exclude'] == TRUE) {
        $linked_fields[$link_path] = $field->field_alias;
      }
    }
    // Iterate through and see if there are other rows that are only off by one
    // for the first field through the nth field.
    foreach ($values as $key => $value) {
      for ($i = $key + 1; $i < count($values); $i++) {
        $array_diff = array_diff_assoc($values[$i], $value);
        $diff_fields = array_keys($array_diff);

        if (count($array_diff) <= 2) {
          // If there is only one distinct value, or no distinct values, then
          // combine with the first row.
          if (count($array_diff) <= 1) {
            $different_field = $diff_fields[0];
          }
          // If there are two distinct values in this row, check to see if they
          // are linked to eachother. We know that the first key of $array_diff
          // would be the $link_path because tokens for linking a field in Views
          // are only available for the fields that came before it.
          elseif (count($array_diff) == 2) {
            if (isset($linked_fields[$diff_fields[0]]) && $linked_fields[$diff_fields[0]] == $diff_fields[1]) {
              $different_field = $diff_fields[1];
            }
          }

          // If there is only one different field and if the user has set the
          // option to combine values on this field...
          if (isset($different_field) && $this->view->field[$different_field]->options['alter']['combine_values'] == TRUE) {
            // And if the row has not already been combined with other rows based
            // on a third field, then combine these two rows.
            if (!isset($combine_rows[$key]) || (count($combine_rows[$key]) == 1 && isset($combine_rows[$key][$different_field]))) {
              $combine_rows[$key][$different_field][] = $i;
            }
          }
        }
      }
    }
    return $combine_rows;
  }
}

