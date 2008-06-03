--TEST--
Testing __unset with private visibility
--FILE--
<?php

class foo {	
	private function __unset($a) {
		print "unset\n";
	}
}

?>
--EXPECTF--
Fatal error: The magic method __unset() must have public visibility and can not be static in %s on line %d
