<?php
// $Id$
class sparql_views_handler_field_link extends sparql_views_handler_field {
  /**
   * Constructor; calls to base object constructor.
   */
  function construct() {
    parent::construct();

    $this->format = $this->definition['format'] ? $this->definition['format'] : FILTER_FORMAT_DEFAULT;

    $this->additional_fields = array();
    if (!is_numeric($this->format)) {
      $this->additional_fields['format'] = array('field' => $this->format);
    }
  }

  function render($values) {
    $output = '';
    $links = array();
    $field_values = $values->{$this->field_alias};
    foreach ($field_values as $value) {
      foreach ($value as $uri => $label) {
        $links[] = array('href'=>$uri, 'title'=>$label);
      }
    }
    $output = theme('links', $links, array('class' => 'links'));

    return $output;
  }

  function element_type() {
    if (isset($this->definition['element type'])) {
      return $this->definition['element type'];
    }

    return 'div';
  }
}