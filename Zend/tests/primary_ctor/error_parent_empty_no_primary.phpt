--TEST--
Primary constructors: parent constructor call requires a primary constructor
--FILE--
<?php
class Base { public function __construct() {} }
class Sub extends Base() {}
?>
--EXPECTF--
Fatal error: Cannot call the parent constructor without a primary constructor in %s on line %d
