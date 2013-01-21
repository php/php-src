--TEST--
ZE2 Tests that a getter/setter defined as private is not accessible externally
--FILE--
<?php

class TimePeriod {
	public $Seconds = 3600;
	
	public $Hours {
		get { return $this->Seconds / 3600; }
		private set { $this->Seconds = $value * 3600; }
	}
}

$o = new TimePeriod();

echo $o->Hours."\n";
$o->Hours = 2;
?>
--EXPECTF--
1

Fatal error: Call to private accessor TimePeriod::$Hours->set() from context '' in %s on line %d
