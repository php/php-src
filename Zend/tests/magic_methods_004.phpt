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
Warning: The magic method __unset() must have public visibility and cannot be static in %s on line %d
