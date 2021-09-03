--TEST--
Bug #70630 (Closure::call/bind() crash with ReflectionFunction->getClosure())
--FILE--
<?php
class a {}
$x = (new ReflectionFunction("substr"))->getClosure();
$x->call(new a);
?>
--EXPECTF--
Warning: Cannot rebind scope of closure created from function in %s on line %d
