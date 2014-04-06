--TEST--
Basic return hints covariance
--FILE--
<?php
class foo {}
class bar extends foo {}
class baz {}

class qux extends baz {
	public function foo() : foo {
		return new bar();
	}
}

$qux = new qux();
var_dump($qux->foo());
?>
--EXPECTF--
object(bar)#%d (%d) {
}


