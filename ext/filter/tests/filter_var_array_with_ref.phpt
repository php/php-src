--TEST--
filter_var_array() on array with reference
--SKIPIF--
<?php if (!extension_loaded("filter")) die("skip"); ?>
--FILE--
<?php

$array = ["123foo"];
$array2 = [&$array];
var_dump(filter_var_array($array2, FILTER_VALIDATE_INT));
var_dump($array);

?>
--EXPECTF--
array(1) {
  [0]=>
  &array(1) {
    [0]=>
    bool(false)
  }
}
array(1) {
  [0]=>
  bool(false)
}
