--TEST--
ReflectionParameter double construct call
--FILE--
<?php

$closure = function (int $x): void {};
$r = new ReflectionParameter($closure, 'x');
var_dump($r);
$r->__construct($closure, 'x');
var_dump($r);
$r->__construct('ord', 'character');
var_dump($r);

?>
--EXPECT--
object(ReflectionParameter)#2 (1) {
  ["name"]=>
  string(1) "x"
}
object(ReflectionParameter)#2 (1) {
  ["name"]=>
  string(1) "x"
}
object(ReflectionParameter)#2 (1) {
  ["name"]=>
  string(9) "character"
}
