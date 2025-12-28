--TEST--
ReflectionExtension double construct call
--FILE--
<?php

$r = new ReflectionExtension('standard');
var_dump($r);
$r->__construct('standard');
var_dump($r);

?>
--EXPECT--
object(ReflectionExtension)#1 (1) {
  ["name"]=>
  string(8) "standard"
}
object(ReflectionExtension)#1 (1) {
  ["name"]=>
  string(8) "standard"
}
