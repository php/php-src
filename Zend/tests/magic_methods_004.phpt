--TEST--
Testing __unset() with protected visibility
--FILE--
<?php

class foo {	
	protected function __unset($a) {
		print "unset\n";
	}
}

?>
--EXPECTF--
Fatal error: The magic method __unset() must have public visibility and can not be static in %s on line %d
