--TEST--
ZE2 Tests that an inherited read-only property cannot be re-declared as non read-only
--FILE--
<?php

class TimePeriod {
	public $Seconds = 3600;
	
	public read-only $Hours {
		get { return $this->Seconds / 3600; }
	}
}

class tp2 extends TimePeriod {
	public $Hours {
		get { return $this->Seconds / 1600; }
	}
}

$o = new TimePeriod();

$o->Hours = 4;
?>
--EXPECTF--
Fatal error: $Hours must be declared read-only as in parent class TimePeriod. in %s on line %d
