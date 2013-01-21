--TEST--
ZE2 Tests that an undefined setter cannot be called and produces an error
--FILE--
<?php

class AccessorTest {
	public $b {
	}
}

$o = new AccessorTest();

$o->b = 10;
?>
--EXPECTF--
Warning: Cannot set property AccessorTest::$b, no setter defined in %s on line %d