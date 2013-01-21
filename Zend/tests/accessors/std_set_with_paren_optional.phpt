--TEST--
ZE2 Tests that a setter may be defined with or without parameters
--FILE--
<?php

class TestClass {
	public function __toString() {
		return "Object [TestClass]"; 
	}
}

class TimePeriod {
	public $_Seconds = 3600;
	
	public $Seconds {
		set { $this->_Seconds = $value; }
	}
	public $Seconds2 {
		set;
	}
	public $Seconds3 {
		set($x) { $this->_Seconds = $x; }
	}
	public $Object {
		set(TestClass $y) { $this->_Seconds = $y; }
	}
}

$o = new TimePeriod();
$tc  = new TestClass();

echo $o->_Seconds."\n";
$o->Seconds = 4000;
echo $o->_Seconds."\n";
$o->Seconds2 = 45;
echo $o->_Seconds."\n";
$o->Seconds3 = 5000;
echo $o->_Seconds."\n";
$o->Object = $tc;
echo $o->_Seconds."\n";

echo "Done\n";
?>
--EXPECTF--
3600
4000
4000
5000
Object [TestClass]
Done