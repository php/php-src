--TEST--
ZE2 Tests that a read-only accessor cannot have unset defined
--FILE--
<?php

class AccessorTest {
	public read-only $b {
		get;
		unset;
	}
}
?>
--EXPECTF--
Fatal error: Cannot define unset for read-only property $b in %s on line %d