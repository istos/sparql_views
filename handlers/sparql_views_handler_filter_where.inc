<?php
// $Id$

/**
 * @file
 * Views filter handler to display UI for WHERE clauses and prepare them for
 * the query.
 */

class sparql_views_handler_filter_where extends sparql_views_handler_filter {

  function admin_summary() {
    /*if (!empty($this->options['exposed'])) {
      return t('exposed');
    }*/
    $output .= ' '. check_plain($this->value['subject'] .' '. $this->value['predicate'] .' '. $this->value['object']);
    return $output;
  }

  function options_submit(&$form, &$form_state) {}

  /**
   * Provide a simple textfield for equality
   */
  function value_form(&$form, &$form_state) {
    $which = 'all';

    $view_name = arg(5);
    $display_name = arg(6);

    if ($which == 'all' || $which == 'value') {
      $form['value'] = array(
        'subject' => array(
          '#type' => 'textfield',
          '#title' => t('Subject'),
          '#size' => 70,
          '#autocomplete_path' => "sparql_views/autocomplete/subject-object/$view_name/$display_name",
          '#default_value' => $this->value['subject'],
        ),
        'predicate' => array(
          '#type' => 'textfield',
          '#title' => t('Attribute (predicate)'),
          '#size' => 70,
          '#autocomplete_path' => 'sparql_views/autocomplete/predicate',
          '#default_value' => $this->value['predicate'],
        ),
        'object' => array(
          '#type' => 'textfield',
          '#title' => t('Value (object)'),
          '#size' => 70,
          '#autocomplete_path' => "sparql_views/autocomplete/subject-object/$view_name/$display_name",
          '#default_value' => $this->value['object'],
        ),
      );
      if (!empty($form_state['exposed']) && !isset($form_state['input'][$identifier])) {
        $form_state['input'][$identifier] = $this->value;
      }

      if ($which == 'all') {
        $form['value'] += array(
          '#process' => array('views_process_dependency'),
        );
      }
    }

    if (!isset($form['value'])) {
      // Ensure there is something in the 'value'.
      $form['value'] = array(
        '#type' => 'value',
        '#value' => NULL,
      );
    }
  }

  function query() {
    if (isset($this->query->where['where'])) {
      if (function_exists('token_replace')) {
        $this->value['subject'] = token_replace($this->value['subject'], $type = 'global', $object = NULL, $leading = '[', $trailing = ']');
        $this->value['object'] = token_replace($this->value['object'], $type = 'global', $object = NULL, $leading = '[', $trailing = ']');
      }
      // Replace tokens.

      $this->query->add_where('where', '.'. $this->value['subject'] .' '. $this->value['predicate'] .' '. $this->value['object']);
    }
    else {
      $this->query->add_where('where', $this->value['subject'] .' '. $this->value['predicate'] .' '. $this->value['object']);
    }
  }
}

