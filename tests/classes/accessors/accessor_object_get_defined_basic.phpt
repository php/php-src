--TEST--
ZE2 Tests that an defined getter has its code executed and return value properly returned upon access
--FILE--
<?php

class AccessorTest {
	public $b {
		get {
			echo "get::\$b called.\n";
			return 10; 
		}
	}
}

$o = new AccessorTest();

echo "\$o->b: ".$o->b."\n";
echo "Done\n";
?>
--EXPECTF--
get::$b called.
$o->b: 10
Done
