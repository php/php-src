--TEST--
Testing __construct and __destruct with Trait
--FILE--
<?php

trait foo {	
	public function __construct() {
		var_dump(__FUNCTION__);
	}
	public function __destruct() {
		var_dump(__FUNCTION__);
	}
}

class bar {
	use foo;
}

new bar;

?>
--EXPECT--
string(11) "__construct"
string(10) "__destruct"
