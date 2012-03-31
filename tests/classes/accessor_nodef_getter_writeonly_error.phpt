--TEST--
ZE2 Tests that a write-only property cannot define a getter
--FILE--
<?php

class TimePeriod {
	public $Seconds = 3600;
	
	public write-only $Hours {
		get;
		set;
	}
}

$o = new TimePeriod();

$o->Hours = 4;
?>
--EXPECTF--
Fatal error: Cannot define getter for write-only property $Hours in %s on line %d
