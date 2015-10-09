--TEST--
Bug #70630 (Closure::call/bind() crash with ReflectionFunction->getClosure())
--FILE--
<?php
class a {}
$x = (new ReflectionFunction("substr"))->getClosure();
$x->call(new a);
?>
--EXPECTF--
Warning: a::substr() expects at least 2 parameters, 0 given in %s on line %d
