--TEST--
Bug #52939 (zend_call_function_array does not respect ZEND_SEND_PREFER_REF)
--FILE--
<?php
$ar1 = array("row1" => 2, "row2" => 1);
var_dump(array_multisort($ar1));
var_dump($ar1);

$ar1 = array("row1" => 2, "row2" => 1);
$args = array(&$ar1);
var_dump(call_user_func_array("array_multisort", $args));
var_dump($ar1);

$ar1 = array("row1" => 2, "row2" => 1);
$args = array($ar1);
var_dump(call_user_func_array("array_multisort", $args));
var_dump($ar1);
?>
--EXPECT--
bool(true)
array(2) {
  ["row2"]=>
  int(1)
  ["row1"]=>
  int(2)
}
bool(true)
array(2) {
  ["row2"]=>
  int(1)
  ["row1"]=>
  int(2)
}
bool(true)
array(2) {
  ["row1"]=>
  int(2)
  ["row2"]=>
  int(1)
}
