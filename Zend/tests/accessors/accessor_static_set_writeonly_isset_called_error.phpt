--TEST--
ZE2 Tests that a write-only static defined setter produces an error if isset() is attempted
--FILE--
<?php

class AccessorTest {
	public write-only static $b {
		set {
			echo "set AccessorTest::\$b Called \$value = {$value}.\n";
		}
	}
}

echo "isset(AccessorTest::\$b) = ".(int)isset(AccessorTest::$b)."\n";
?>
--EXPECTF--
Fatal error: Cannot call isset on write-only static property (AccessorTest::b). in %s on line %d
