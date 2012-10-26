--TEST--
ZE2 Tests that a accessor declared as write-only cannot declare isset
--FILE--
<?php

class AccessorTest {
	public write-only $b {
		set;
		isset;
	}
}

?>
--EXPECTF--
Fatal error: Cannot define isset for write-only property $b in %s on line %d