--TEST--
Bug #64821 Custom Exceptions crash when internal properties overridden (variation 2)
--FILE--
<?php

class a extends exception {
	public function __construct() {
		$this->line = array();
	}
}

throw new a;

?>
--EXPECTF--
a: (empty message) in %s on line 0
Stack trace:
#0 {main}
