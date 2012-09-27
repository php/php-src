--TEST--
ZE2 Tests that access to a static undefined parent getter produces compile error (pass_two() functionality)
--FILE--
<?php

class ParentAccessorTest {
	public static $a {
		set {
			echo "set ParentAccessorTest::\$a Called \$value = {$value}.\n";
		}
	}
}

class AccessorTest extends ParentAccessorTest {
	public static $b {
		get {
			echo "get AccessorTest::\$b Called.\n";
			return parent::$a;
		}
		set {
			echo "set AccessorTest::\$b Called \$value = {$value}.\n";
		}
	}
}

echo AccessorTest::$b;
?>
--EXPECTF--
Fatal error: Cannot get property parent::$a, no getter defined. in %s on line %d
