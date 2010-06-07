<?php
// $Id: sparql_views_handler_field_markup.inc,v 1.1 2009/11/01 10:48:24 dereine Exp $
class sparql_views_handler_field_markup extends sparql_views_handler_field {
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
    $value = $values->{$this->field_alias};
    if (is_array($value)) {
      foreach ($value as $item) {
        $output .= $item . ', ';
      }
    }
    else {
      $output = $value;
    }
    $format = is_numeric($this->format) ? $this->format : $values->{$this->aliases['format']};
    return check_markup($output, $format, FALSE);
  }

  function element_type() {
    if (isset($this->definition['element type'])) {
      return $this->definition['element type'];
    }

    return 'div';
  }
}