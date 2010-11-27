<?php
// $Id: content_handler_argument.inc,v 1.1.2.2 2008/10/04 13:14:21 karens Exp $

/**
 * @file
 * The subclass simply adds properties,
 * for field-specific subclasses to use if they need to.
 */
class sparql_views_handler_argument extends views_handler_argument {

  function construct() {
    parent::construct();
  }
  
  function option_definition() {
    $options = parent::option_definition();
    $options['replacement_type'] = array('default' => 'string');
    return $options;
  }
  
  function options_form(&$form, &$form_state) {
    $form['replacement_type'] = array(
      '#type' => 'radios',
      '#title' => 'Replacement type',
      '#default_value' => $this->options['replacement_type'],
      '#required' => TRUE,
    );
    $options = array('string' => 'String');

    $form['replacement_type']['#options'] = $options;

    parent::options_form(&$form, &$form_state);
    return $form;
  }

  function query() {
    $argument = $this->argument;
    
    if ($this->options['replacement_type'] == 'string') {
      $this->query->add_filter('regex', $this->field, $argument);
    }
    else if ($this->options['replacement_type'] == 'url') {
      $argument = '<' . $argument . '>';
      $this->query->add_param($this->field, $argument);
    }
  }
}