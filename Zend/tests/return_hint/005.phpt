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
Fatal error: the function foo was expected to return foo and returned baz in %s on line %d

