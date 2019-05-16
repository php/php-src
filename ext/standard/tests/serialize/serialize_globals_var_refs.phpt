--TEST--
Reference IDs should be correctly generated when $GLOBALS is serialized
--FILE--
<?php

$obj = new stdClass;
$obj2 = new stdClass;
$obj2->obj = $obj;
$s = serialize($GLOBALS);
$globals = unserialize($s);
var_dump($obj);
var_dump($obj2);

?>
--EXPECT--
object(stdClass)#1 (0) {
}
object(stdClass)#2 (1) {
  ["obj"]=>
  object(stdClass)#1 (0) {
  }
}
