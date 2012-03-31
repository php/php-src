--TEST--
ZE2 Tests that an accessor cannot be both read-only and write-only
--FILE--
<?php

class TimePeriod {
	public $Seconds = 3600;
	
	public read-only write-only $Hours {
		private get { return $this->Seconds / 3600; }
		private set { $this->Seconds = $value * 3600; }
	}
}

$o = new TimePeriod();

echo $o->Hours."\n";
?>
--EXPECTF--
Fatal error: read-only and write-only modifiers are mutually exclusive in %s on line %d
