--TEST--
ReflectionProperty double construct call
--FILE--
<?php

$r = new ReflectionProperty(Exception::class, 'message');
var_dump($r);
$r->__construct(Exception::class, 'message');
var_dump($r);

?>
--EXPECT--
object(ReflectionProperty)#1 (2) {
  ["name"]=>
  string(7) "message"
  ["class"]=>
  string(9) "Exception"
}
object(ReflectionProperty)#1 (2) {
  ["name"]=>
  string(7) "message"
  ["class"]=>
  string(9) "Exception"
}
