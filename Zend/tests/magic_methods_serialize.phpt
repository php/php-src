--TEST--
__serialize declaration
--FILE--
<?php
class Foo {
	static function __serialize($arguments) {}
}
?>
--EXPECTF--
Warning: The magic method Foo::__serialize() must have public visibility and cannot be static in %s on line %d

Fatal error: Method Foo::__serialize() cannot take arguments in %s on line %d
