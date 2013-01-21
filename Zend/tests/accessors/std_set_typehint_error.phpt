--TEST--
ZE2 Tests that a setter defined with a typehint produces error on invalid parameter pass
--FILE--
<?php

class TestClass {
	public function __toString() {
		return "Object [TestClass]"; 
	}
}

class TimePeriod {
	public $Object {
		set(TestClass2 $y) { $this->_Seconds = $y; }
	}
}

$o = new TimePeriod();
$tc  = new TestClass();

$o->Object = $tc;
?>
--EXPECTF--
Catchable fatal error: Argument 1 passed to TimePeriod::$Object->set() must be an instance of TestClass2, instance of TestClass given in %s on line %d
