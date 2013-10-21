<?php
class Lib_Head extends PBMessage
{
  var $wired_type = PBMessage::WIRED_LENGTH_DELIMITED;
  public function __construct($reader=null)
  {
    parent::__construct($reader);
    $this->fields["1"] = "PBInt";
    $this->values["1"] = "";
    $this->fields["2"] = "PBInt";
    $this->values["2"] = "";
    $this->fields["3"] = "PBInt";
    $this->values["3"] = "";
  }
  function ver()
  {
    return $this->_get_value("1");
  }
  function set_ver($value)
  {
    return $this->_set_value("1", $value);
  }
  function cmd()
  {
    return $this->_get_value("2");
  }
  function set_cmd($value)
  {
    return $this->_set_value("2", $value);
  }
  function seq()
  {
    return $this->_get_value("3");
  }
  function set_seq($value)
  {
    return $this->_set_value("3", $value);
  }
}
?>