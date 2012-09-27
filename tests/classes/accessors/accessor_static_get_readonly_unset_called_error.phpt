--TEST--
ZE2 Tests that a read-only static defined getter produces an error if unset() is attempted
--FILE--
<?php

class AccessorTest {
	public read-only static $b {
		get {
			echo "get AccessorTest::\$b Called.\n";
			return 5;
		}
	}
}

echo "unset(AccessorTest::\$b)\r\n";
unset(AccessorTest::$b);	/* Above echo should not occur, unset() is a compile time error */
?>
--EXPECTF--
Fatal error: Cannot call unset on read-only static property (AccessorTest::$b). in %s on line %d