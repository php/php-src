--TEST--
ReflectionObject double construct call
--FILE--
<?php

$obj = new stdClass;
$r = new ReflectionObject($obj);
var_dump($r);
$r->__construct($obj);
var_dump($r);

?>
--EXPECT--
object(ReflectionObject)#2 (1) {
  ["name"]=>
  string(8) "stdClass"
}
object(ReflectionObject)#2 (1) {
  ["name"]=>
  string(8) "stdClass"
}
