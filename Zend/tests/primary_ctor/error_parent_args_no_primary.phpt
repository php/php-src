--TEST--
Primary constructors: parent constructor arguments require a primary constructor
--FILE--
<?php
class Base { public function __construct(public int $id) {} }
class Sub extends Base(5) {}
?>
--EXPECTF--
Fatal error: Cannot pass arguments to the parent constructor without a primary constructor in %s on line %d
