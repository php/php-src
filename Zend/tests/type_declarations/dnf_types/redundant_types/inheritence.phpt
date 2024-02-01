--TEST--
Intersection with child class
--FILE--
<?php

interface  X {}
class A {}
class B extends A {}

function test(): (A&X)|(B&X) {}

?>
===DONE===
--EXPECT--
===DONE===
