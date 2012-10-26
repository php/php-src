--TEST--
ZE2 Tests that access to a static undefined self getter produces compile error (pass_two() functionality)
--FILE--
<?php

class AccessorTest {
	public static $a {
		set;
	}
	public static $b {
		get {
			echo "get AccessorTest::\$b Called.\n";
			return self::$a;
		}
	}
}

echo AccessorTest::$b;
?>
--EXPECTF--
Fatal error: Cannot get property self::$a, no getter defined. in %s on line %d
