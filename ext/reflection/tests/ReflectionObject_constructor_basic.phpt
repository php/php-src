--TEST--
ReflectionObject::__construct - basic function test
--FILE--
<?php
$r1 = new ReflectionObject(new stdClass);
var_dump($r1);

class C { }
$myInstance = new C;
$r2 = new ReflectionObject($myInstance);
var_dump($r2);

$r3 = new ReflectionObject($r2);
var_dump($r3);
?>
--EXPECTF--
object(ReflectionObject)#%d (1) {
  ["name"]=>
  string(8) "stdClass"
}
object(ReflectionObject)#%d (1) {
  ["name"]=>
  string(1) "C"
}
object(ReflectionObject)#%d (1) {
  ["name"]=>
  string(16) "ReflectionObject"
}
