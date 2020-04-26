--TEST--
__set_state first argument must be an array
--FILE--
<?php
class Foo {
	public function __set_state(string $name) {}
}
?>
--EXPECTF--
Fatal error: Type of first argument of Foo::__set_state() must be array in %s on line %d