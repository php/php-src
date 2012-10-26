--TEST--
ZE2 Tests that a read-only property cannot define a setter
--FILE--
<?php

class TimePeriod {
	public $Seconds = 3600;
	
	public read-only $Hours {
		get;
		set;
	}
}

$o = new TimePeriod();

$o->Hours = 4;
?>
--EXPECTF--
Fatal error: Cannot define setter for read-only property $Hours in %s on line %d
