--TEST--
Test typed disallow incorrect type initial value (array)
--FILE--
<?php
class Foo {
	public array $bar = null;
}
?>
--EXPECTF--
Fatal error: Type of Foo::$bar must be array, null used in %s on line 3



