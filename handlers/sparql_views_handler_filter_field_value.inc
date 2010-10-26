<?php
// $Id$

/**
 * @file
 * Views filter handler to display UI for WHERE clauses and prepare them for
 * the query.
 */

class sparql_views_handler_filter_field_value extends views_handler_filter_string{

  function option_definition() {
    $options = parent::option_definition();

    $options['type'] = array('default' => 'textfield');
    $options['limit'] = array('default' => TRUE);
    $options['error_message'] = array('default' => TRUE);

    return $options;
  }
  
  

  function admin_summary() {
    if (!empty($this->options['exposed'])) {
      return t('exposed');
    }
    $output .= ' '. check_plain($this->value['subject'] .' '. $this->value['predicate'] .' '. $this->value['object']);
    return $output;
  }

  function options_submit(&$form, &$form_state) {}

  
  function value_form(&$form, &$form_state) {
    $which = 'all';

    $view_name = arg(5);
    $display_name = arg(6);

    if ($which == 'all' || $which == 'value') {
      $form['value'] = array(
        'field' => array(
          '#type' => 'textfield',
          '#title' => t('Field'),
          '#size' => 70,
          '#autocomplete_path' => "sparql_views/autocomplete/subject-object/$view_name/$display_name",
          '#default_value' => $this->value['field'],
        ),
        'value' => array(
          '#type' => 'textfield',
          '#title' => t('Value'),
          '#size' => 70,
          '#default_value' => $this->value['value'],
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
    // Replace tokens.
    if (function_exists('token_replace')) {
      $this->value['field'] = token_replace($this->value['field'], $type = 'global', $object = NULL, $leading = '[', $trailing = ']');
      $this->value['value'] = token_replace($this->value['value'], $type = 'global', $object = NULL, $leading = '[', $trailing = ']');
    }
    dpm($this);
    $this->query->add_param($this->value['field'], $this->value['value']);
  }
}
