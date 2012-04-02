--TEST--
ZE2 Tests that an inherited write-only property cannot be re-declared as non write-only
--FILE--
<?php

class TimePeriod {
	public $Seconds = 3600;
	
	public write-only $Hours {
		set { $this->Seconds = $value; }
	}
}

class tp2 extends TimePeriod {
	public $Hours {
		set { $this->Seconds = $value; }
	}
}

$o = new TimePeriod();

$o->Hours = 4;
?>
--EXPECTF--
Fatal error: $Hours must be declared write-only as in parent class TimePeriod. in %s on line %d
