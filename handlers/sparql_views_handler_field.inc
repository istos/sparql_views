<?php
// $Id$

class sparql_views_handler_field extends views_handler_field {

  /**
   * Called to add the field to a query.
   */
  function query() {
    $this->field_alias = $this->real_field;
  }

  function pre_render(&$values) {
    static $sparql_views_storage;

    // Run through and see if there are other rows that are only off by one for
    // the first field through the nth field.
    reset($this->view->field);
    $first_key = key($this->view->field);
    if ($this->field_alias == $first_key && count($values) > 1) {
      foreach ($values as $key => $value) {
        for ($i = $key + 1; $i < count($values); $i++) {
          if (count(array_diff_assoc($values[$i], $value) < 3)) {
            $sparql_views_storage['combine'][$key]['person_name'] = $i;
          }
        }
      }
    }

    foreach ($values as $key => $value) {
      $items = array();
      $field = $value[$this->field_alias];
      if (!isset($sparql_views_storage['field_values'][$this->field_alias][$key])) {
        $sparql_views_storage['field_values'][$this->field_alias][$key] = $field;
      }
      $token = $this->options['alter']['path'];
      if (!empty($token)) {
        $token_fieldname = preg_replace('/\[|\]/', '', $token);
        $link = isset($sparql_views_storage['field_values'][$token_fieldname][$key]) ? $sparql_views_storage['field_values'][$token_fieldname][$key] : $value[$token_fieldname];
      }
      if (!empty($field)) {
        $items[] = array('field' => $field, 'token' => array($token => $link));
      }

      if (isset($sparql_views_storage['combine'][$key][$this->field_alias])) {
        $key2 = $sparql_views_storage['combine'][$key]['person_name'];
        if (!isset($sparql_views_storage['field_values'][$this->field_alias][$key2])) {
          $field = $values[$key2][$this->field_alias];
          $sparql_views_storage['field_values'][$this->field_alias][$key2] = $field;
        }
        $token = $this->options['alter']['path'];
        if (!empty($token)) {
          $token_fieldname = preg_replace('/\[|\]/', '', $token);
          $link = isset($sparql_views_storage['field_values'][$token_fieldname][$key2]) ? $sparql_views_storage['field_values'][$token_fieldname][$key2] : $values[$key2][$token_fieldname];
        }
        if (!empty($field)) {
          $items[] = array('field' => $field, 'token' => array($token => $link));
        }
      }
      $values[$key2]['rendered'] = TRUE;
      $values[$key][$this->field_alias] = $items;
    }

    // If this is the last pass through this function, unset all of the values
    // that have been combined with another row.
    $last_field = end($this->view->field);
    if ($last_field->field_alias == $this->field_alias) {
      foreach ($values as $key => $value) {
        if ($value['rendered'] == TRUE) {
          unset($values[$key]);
        }
      }
    }
  }

  function get_items($values) {
    $items = $values[$this->field_alias];
    return $items;
  }

  // We need to include this override in order to use get_items.
  function render_item($count, $item) {
    return $item;
  }

  function render_items($items) {
    $this->options['type'] = 'separator';
    $this->options['separator'] = ' || ';
    if (!empty($items)) {
      if ($this->options['type'] == 'separator') {
        return implode(check_plain($this->options['separator']), $items);
      }
      else {
        return theme('item_list', $items, NULL, $this->options['type']);
      }
    }
  }

  /**
   * Render a field using advanced settings.
   *
   * This renders a field normally, then decides if render-as-link and
   * text-replacement rendering is necessary.
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
          $rendered = $this->render_item($count, $item);
          $this->last_render = $rendered['field'];
          $this->original_value = $this->last_render;

          $alter = $item + $this->options['alter'];
          $items[] = $this->render_text($alter);
        }

        $value = $this->render_items($items);
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

    return $this->last_render;
  }

  /**
   * Get the 'render' tokens to use for advanced rendering.
   *
   * This runs through all of the fields and arguments that
   * are available and gets their values. This will then be
   * used in one giant str_replace().
   */
  function get_render_tokens($item) {
    $tokens = array();
    if (!empty($this->view->build_info['substitutions'])) {
      $tokens = $this->view->build_info['substitutions'];
    }
    $count = 0;
    foreach ($this->view->display_handler->get_handlers('argument') as $arg => $handler) {
      $token = '%'. ++$count;
      if (!isset($tokens[$token])) {
        $tokens[$token] = '';
      }

      $tokens['!'. $count] = isset($this->view->args[$count - 1]) ? check_plain($this->view->args[$count - 1]) : '';
    }

    // Now add replacements for our fields.
    foreach ($this->view->display_handler->get_handlers('field') as $field => $handler) {
      if (isset($handler->last_render)) {
        $tokens["[$field]"] = $handler->last_render;
      }
      else {
        $tokens["[$field]"] = '';
      }
      $tokens = array_merge($tokens, $item['token']);
      $this->add_self_tokens($tokens, $item);

      // We only use fields up to (and including) this one.
      if ($field == $this->options['id']) {
        break;
      }
    }
    return $tokens;
  }
}

