--TEST--
ZE2 Tests that an undefined static getter produces an error indicating it's non-existence
--FILE--
<?php

class AccessorTest {
	public static $b {
	}
}

echo "AccessorTest::\$b: ".AccessorTest::$b."\n";
?>
--EXPECTF--
Fatal error: Access to undeclared static property: AccessorTest::$b in %s on line %d