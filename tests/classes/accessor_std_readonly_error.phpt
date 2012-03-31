--TEST--
ZE2 Tests that a read-only property cannot be set
--FILE--
<?php

class TimePeriod {
	public $Seconds = 3600;
	
	public read-only $Hours {
		get { return $this->Seconds / 3600; }
	}
}

$o = new TimePeriod();

$o->Hours = 4;
?>
--EXPECTF--
Fatal error: Cannot set read-only property TimePeriod::$Hours. in %s on line %d
