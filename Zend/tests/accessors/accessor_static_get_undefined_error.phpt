--TEST--
ZE2 Tests that an undefined static getter produces an error indicating it's non-existence
--FILE--
<?php

class AccessorTest {
	public static $b {
		set;
	}
}

echo "AccessorTest::\$b: ".AccessorTest::$b."\n";
?>
--EXPECTF--
Fatal error: Cannot get property AccessorTest::$b, no getter defined. in %s on line %d