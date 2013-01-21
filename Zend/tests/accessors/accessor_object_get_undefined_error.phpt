--TEST--
ZE2 Tests that a non-defined getter produces an error upon access
--FILE--
<?php

class AccessorTest {
	public $b {
		set;
	}
}

$o = new AccessorTest();

echo "\$o->b: ".$o->b."\n";
echo "Done\n";
?>
--EXPECTF--
Warning: Cannot get property AccessorTest::$b, no getter defined in %s on line %d
$o->b: 
Done
