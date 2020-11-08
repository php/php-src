--TEST--
ASSIGN_OBJ should not return reference
--FILE--
<?php

$obj = new stdClass;
$obj->ref =& $ref;
$obj->val = $obj->ref = 42;
var_dump($obj);

?>
--EXPECT--
object(stdClass)#1 (2) {
  ["ref"]=>
  &int(42)
  ["val"]=>
  int(42)
}
