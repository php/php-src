--TEST--
ZE2 Tests that a write-only defined setter produces an error if isset() is attempted
--FILE--
<?php

class AccessorTest {
	public write-only $b {
		set {
			echo "set AccessorTest::\$b Called \$value = {$value}.\n";
		}
	}
}

$o = new AccessorTest();

echo "isset(\$o->b) = ".(int)isset($o->b)."\n";
?>
--EXPECTF--
Fatal error: Cannot isset write-only property AccessorTest::$b. in %s on line %d
