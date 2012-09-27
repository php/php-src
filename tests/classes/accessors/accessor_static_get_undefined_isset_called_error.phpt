--TEST--
ZE2 Tests that an undefined static getter produces an error when attempting to isset()
--FILE--
<?php

class AccessorTest {
	public static $b {
		set;
	}
}

echo "isset(AccessorTest::\$b) = ".(int)isset(AccessorTest::$b)."\n";
?>
--EXPECTF--
Fatal error: Cannot isset property AccessorTest::$b, no getter defined. in %s on line %d