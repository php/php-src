--TEST--
Checking error message when a trait's interface implementation doesn't match the interface
--FILE--
<?php

interface baz {
	public function abc($param);
}

trait bar implements baz {
	public function abc() {
	}
}

?>
--EXPECTF--
Fatal error: Declaration of bar::abc() must be compatible with baz::abc($param) in %s on line %d
