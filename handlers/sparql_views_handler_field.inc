<?php
// $Id$

class sparql_views_handler_field extends views_handler_field {

  /**
   * Called to add the field to a query.
   */
  function query() {
    // $this->ensure_my_table();
    // Add the field.
    // $this->field_alias = $this->query->add_field($this->table_alias, $this->real_field);
    // $this->add_additional_fields();
    
    $this->field_alias = $this->real_field;
  }

}
