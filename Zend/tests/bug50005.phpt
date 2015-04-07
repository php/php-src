--TEST--
Bug #50005 (Throwing through Reflection modified Exception object makes segmentation fault)
--FILE--
<?php

class a extends exception {
	public function __construct() {
		$this->file = null;
	}
}

throw new a;

?>
--EXPECTF--
a: (empty message) in Unknown on line %d
Stack trace:
#0 {main}
