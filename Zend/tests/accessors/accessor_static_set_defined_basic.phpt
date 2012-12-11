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

echo "AccessorTest::\$b = 12;\n";
AccessorTest::$b = 12;
echo "Done\n";
?>
--EXPECTF--
AccessorTest::$b = 12;
set AccessorTest::$b Called $value = 12.
Done
