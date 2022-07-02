--TEST--
Intersection with child class
--FILE--
<?php

class A {}
class B extends A {}

function test(): A&B {
}

?>
===DONE===
--EXPECT--
===DONE===
