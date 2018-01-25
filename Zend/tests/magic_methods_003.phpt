--TEST--
Testing __unset declaring as static
--FILE--
<?php

class foo {
	static function __unset($a) {
		print "unset\n";
	}
}

?>
--EXPECTF--
Warning: The magic method __unset() must have public visibility and cannot be static in %s on line %d
