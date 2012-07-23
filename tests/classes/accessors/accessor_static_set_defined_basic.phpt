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

AccessorTest::$b = 12;
echo "AccessorTest::\$b = 12;\n";
echo "AccessorTest::\$b: ".AccessorTest::$b."\n";
echo "Done\n";
?>
--EXPECTF--
set AccessorTest::$b Called $value = 12.
AccessorTest::$b = 12;
Done
