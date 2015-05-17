--TEST--
Bug #64821 Custom Exceptions crash when internal properties overridden (variation 3)
--FILE--
<?php

class a extends exception {
	public function __construct() {
		$this->line = array();
		$this->file = NULL;
	}
}

throw new a;

?>
--EXPECTF--
a: (empty message) in Unknown on line 0
Stack trace:
#0 {main}
