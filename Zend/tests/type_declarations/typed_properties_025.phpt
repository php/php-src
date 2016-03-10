--TEST--
Test typed properties disallow mixing typed and untyped declarations
--FILE--
<?php
class Foo {
	public $bar,
			int	$qux;
}
?>
--EXPECTF--
Fatal error: Typed property Foo::$qux must not be mixed with untyped properties in %s on line 3





