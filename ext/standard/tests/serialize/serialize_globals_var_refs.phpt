--TEST--
Reference IDs should be correctly generated when $GLOBALS is serialized
--FILE--
<?php

$obj = new StdClass;
$obj2 = new StdClass;
$obj2->obj = $obj;
$s = serialize($GLOBALS);
$globals = unserialize($s);
var_dump($obj);
var_dump($obj2);

?>
--EXPECT--
object(StdClass)#1 (0) {
}
object(StdClass)#2 (1) {
  ["obj"]=>
  object(StdClass)#1 (0) {
  }
}
