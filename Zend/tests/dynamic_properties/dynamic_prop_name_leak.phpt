--TEST--
Dynamic prop name with type conversion in reference position should not leak
--FILE--
<?php
$obj = new stdClass;
$name = 0.0;
$ref =& $obj->$name;
var_dump($obj);
?>
--EXPECT--
object(stdClass)#1 (1) {
  ["0"]=>
  &NULL
}
