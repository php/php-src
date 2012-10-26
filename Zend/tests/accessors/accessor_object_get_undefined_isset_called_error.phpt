--TEST--
ZE2 Tests that a undefined getter produces an error upon isset()
--FILE--
<?php

class AccessorTest {
	public $b {
		set;
	}
}

$o = new AccessorTest();

echo "isset(\$o->b) = ".(int)isset($o->b)."\n";
?>
--EXPECTF--
Fatal error: Cannot get property AccessorTest::$b, no getter defined. in %s on line %d