--TEST--
Test typed properties disallow mixing typed and untyped declarations
--FILE--
<?php
class Foo {
	public int $bar,
				$qux;
}
?>
--EXPECTF--
Fatal error: Property Foo::$qux is missing type information in %s on line 3





