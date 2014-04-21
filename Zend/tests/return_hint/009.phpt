--TEST--
Basic return hints covariance interfaces errors
--FILE--
<?php
interface foo {}
interface bar {
	public function foo() : foo;
}

class qux implements bar {
	public function foo() : array {
		return new qux();
	}
}

$qux = new qux();
var_dump($qux->foo());
?>
--EXPECTF--
Fatal error: Declaration of qux::foo should be compatible with bar::foo() : foo, return type mismatch in %s on line %d


