--TEST--
Abstract function must be implemented
--FILE--
<?php

abstract class ParentClass {
    abstract public function f();
}

$o = new class extends ParentClass {};
?>
--EXPECTF--
Fatal error: Class ParentClass@anonymous%0%s:7$0 must implement 1 abstract method (ParentClass::f) in %s on line %d
