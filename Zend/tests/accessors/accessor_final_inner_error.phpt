--TEST--
ZE2 Tests that a asymmetrical final setter cannot be over-ridden.
	Also tests that a non-final getter can be over-ridden
--FILE--
<?php

class TimePeriod {
	public $Seconds = 3600;
	
	public $Hours {
		get;
		final set;
	}
}

class TimePeriod2 extends TimePeriod {
	public $Hours {
		get;
		set;
	}
}

$o = new TimePeriod2();

echo $o->Hours."\n";
?>
--EXPECTF--
Fatal error: Cannot override final accessor TimePeriod::$Hours->set() in %s on line %d
