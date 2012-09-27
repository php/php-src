--TEST--
ZE2 Tests that an undefined setter produces an error on unset()
--FILE--
<?php

class AccessorTest {
	public $b {
		get;
	}
}

$o = new AccessorTest();

unset($o->b);
?>
--EXPECTF--
Fatal error: Cannot set property AccessorTest::$b, no setter defined. in %s on line %d