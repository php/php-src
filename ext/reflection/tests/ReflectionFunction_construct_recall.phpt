--TEST--
ReflectionFunction constructor called manually after instantiation
--FILE--
<?php

$fn1 = fn ($a) => $a . 'hello';

$r = new ReflectionFunction($fn1);
var_dump($r);
$r->__construct('strpos');
var_dump($r);

?>
--EXPECTF--
object(ReflectionFunction)#2 (1) {
  ["name"]=>
  string(%d) "%s"
}
object(ReflectionFunction)#2 (1) {
  ["name"]=>
  string(6) "strpos"
}
