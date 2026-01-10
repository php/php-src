--TEST--
ReflectionConstant double construct call
--FILE--
<?php

const C1 = 42;
const C2 = 43;

$r = new ReflectionConstant('C' . mt_rand(1, 1));
var_dump($r);

$r->__construct('C' . mt_rand(2, 2));
var_dump($r);

?>
--EXPECT--
object(ReflectionConstant)#1 (1) {
  ["name"]=>
  string(2) "C1"
}
object(ReflectionConstant)#1 (1) {
  ["name"]=>
  string(2) "C2"
}
