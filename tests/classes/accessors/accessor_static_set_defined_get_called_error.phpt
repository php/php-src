--TEST--
ZE2 Tests that a static defined setter has it's code called.
--FILE--
<?php

class AccessorTest {
	public static $b {
		set {
			echo "set AccessorTest::\$b Called \$value = {$value}.\n";
		}
	}
}

echo AccessorTest::$b;
?>
--EXPECTF--
Fatal error: Cannot get property AccessorTest::$b, no getter defined. in %s on line %d