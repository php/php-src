--TEST--
ZE2 Tests that a read-only defined getter produces an error if unset() is attempted
--FILE--
<?php

class AccessorTest {
	public read-only $b {
		get {
			echo "get AccessorTest::\$b Called.\n";
			return 5;
		}
	}
}

$o = new AccessorTest();
unset($o->b);
?>
--EXPECTF--
Fatal error: Cannot unset read-only property AccessorTest::$b. in %s on line %d