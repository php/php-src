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
Fatal error: Class ParentClass@anonymous%0%s must implement 1 abstract method (ParentClass::f) in %sgh15994.php on line 7
