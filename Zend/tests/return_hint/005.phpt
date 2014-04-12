--TEST--
Basic return hints methods
--FILE--
<?php
class foo {}
class bar {}
class baz {}

class qux extends baz {
	public function foo() : foo {
		return new baz();
	}
}

$qux = new qux();
$qux->foo();
?>
--EXPECTF--
Catchable fatal error: the function qux::foo was expected to return an object of class foo and returned an object of class baz in %s on line %d


