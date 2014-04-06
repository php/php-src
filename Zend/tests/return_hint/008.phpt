--TEST--
Basic return hints covariance interfaces 
--FILE--
<?php
interface foo {}
interface bar extends foo {
	public function foo() : foo;
}


class qux implements bar {
	public function foo() : bar {
		return new qux();
	}
}

$qux = new qux();
var_dump($qux->foo());
?>
--EXPECTF--
object(qux)#%d (%d) {
}


