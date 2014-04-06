--TEST--
Basic return hints covariance interfaces nulls
--FILE--
<?php
interface foo {}
interface bar extends foo {
	public function foo() : foo;
}

class qux implements bar {
	public function foo() : ?foo {
		return null;
	}
}

$qux = new qux();
var_dump($qux->foo());
?>
--EXPECT--
NULL
