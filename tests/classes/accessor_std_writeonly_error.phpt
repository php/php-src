--TEST--
ZE2 Tests that a write-only property cannot be read
--FILE--
<?php

class TimePeriod {
	public $Seconds = 3600;
	
	public write-only $Hours {
		set { $this->Seconds = $value * 3600; }
	}
}

$o = new TimePeriod();

echo $o->Hours;
?>
--EXPECTF--
Fatal error: Cannot get write-only property TimePeriod::$Hours. in %s on line %d
