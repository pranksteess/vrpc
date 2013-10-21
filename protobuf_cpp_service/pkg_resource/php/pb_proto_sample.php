<?php
class PayInfo extends PBMessage
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
    $this->fields["4"] = "PBInt";
    $this->values["4"] = "";
    $this->fields["5"] = "PBString";
    $this->values["5"] = "";
    $this->fields["6"] = "PBString";
    $this->values["6"] = "";
  }
  function uint32_days()
  {
    return $this->_get_value("1");
  }
  function set_uint32_days($value)
  {
    return $this->_set_value("1", $value);
  }
  function uint32_price()
  {
    return $this->_get_value("2");
  }
  function set_uint32_price($value)
  {
    return $this->_set_value("2", $value);
  }
  function uint32_discount()
  {
    return $this->_get_value("3");
  }
  function set_uint32_discount($value)
  {
    return $this->_set_value("3", $value);
  }
  function uint32_pay_flag()
  {
    return $this->_get_value("4");
  }
  function set_uint32_pay_flag($value)
  {
    return $this->_set_value("4", $value);
  }
  function str_pay_token()
  {
    return $this->_get_value("5");
  }
  function set_str_pay_token($value)
  {
    return $this->_set_value("5", $value);
  }
  function str_product_id()
  {
    return $this->_get_value("6");
  }
  function set_str_product_id($value)
  {
    return $this->_set_value("6", $value);
  }
}
class ReqPricing extends PBMessage
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
    $this->fields["4"] = "PBString";
    $this->values["4"] = "";
  }
  function uint64_uin()
  {
    return $this->_get_value("1");
  }
  function set_uint64_uin($value)
  {
    return $this->_set_value("1", $value);
  }
  function uint32_item_id()
  {
    return $this->_get_value("2");
  }
  function set_uint32_item_id($value)
  {
    return $this->_set_value("2", $value);
  }
  function uint32_plat_id()
  {
    return $this->_get_value("3");
  }
  function set_uint32_plat_id($value)
  {
    return $this->_set_value("3", $value);
  }
  function str_pay_token()
  {
    return $this->_get_value("4");
  }
  function set_str_pay_token($value)
  {
    return $this->_set_value("4", $value);
  }
}
class ReqGetItemInfo extends PBMessage
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
    $this->fields["4"] = "PBInt";
    $this->values["4"] = "";
  }
  function uint64_uin()
  {
    return $this->_get_value("1");
  }
  function set_uint64_uin($value)
  {
    return $this->_set_value("1", $value);
  }
  function uint32_item_id()
  {
    return $this->_get_value("2");
  }
  function set_uint32_item_id($value)
  {
    return $this->_set_value("2", $value);
  }
  function uint32_plat_id()
  {
    return $this->_get_value("3");
  }
  function set_uint32_plat_id($value)
  {
    return $this->_set_value("3", $value);
  }
  function uint32_price_lock()
  {
    return $this->_get_value("4");
  }
  function set_uint32_price_lock($value)
  {
    return $this->_set_value("4", $value);
  }
}
class RspGetItemInfo extends PBMessage
{
  var $wired_type = PBMessage::WIRED_LENGTH_DELIMITED;
  public function __construct($reader=null)
  {
    parent::__construct($reader);
    $this->fields["1"] = "PBInt";
    $this->values["1"] = "";
    $this->fields["2"] = "PayInfo";
    $this->values["2"] = array();
    $this->fields["3"] = "PBInt";
    $this->values["3"] = "";
    $this->fields["4"] = "PBInt";
    $this->values["4"] = "";
  }
  function int32_ret()
  {
    return $this->_get_value("1");
  }
  function set_int32_ret($value)
  {
    return $this->_set_value("1", $value);
  }
  function rpt_msg_pay_info($offset)
  {
    return $this->_get_arr_value("2", $offset);
  }
  function add_rpt_msg_pay_info()
  {
    return $this->_add_arr_value("2");
  }
  function set_rpt_msg_pay_info($index, $value)
  {
    $this->_set_arr_value("2", $index, $value);
  }
  function remove_last_rpt_msg_pay_info()
  {
    $this->_remove_last_arr_value("2");
  }
  function rpt_msg_pay_info_size()
  {
    return $this->_get_arr_size("2");
  }
  function uint32_pay_info_default_index()
  {
    return $this->_get_value("3");
  }
  function set_uint32_pay_info_default_index($value)
  {
    return $this->_set_value("3", $value);
  }
  function uint32_item_type()
  {
    return $this->_get_value("4");
  }
  function set_uint32_item_type($value)
  {
    return $this->_set_value("4", $value);
  }
}
class RspPricing extends PBMessage
{
  var $wired_type = PBMessage::WIRED_LENGTH_DELIMITED;
  public function __construct($reader=null)
  {
    parent::__construct($reader);
    $this->fields["1"] = "PBInt";
    $this->values["1"] = "";
    $this->fields["2"] = "PBInt";
    $this->values["2"] = "";
  }
  function int32_ret()
  {
    return $this->_get_value("1");
  }
  function set_int32_ret($value)
  {
    return $this->_set_value("1", $value);
  }
  function uint32_pay_price()
  {
    return $this->_get_value("2");
  }
  function set_uint32_pay_price($value)
  {
    return $this->_set_value("2", $value);
  }
}
?>