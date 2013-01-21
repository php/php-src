--TEST--
ZE2 Tests that a final getter/setter cannot be over-ridden
--FILE--
<?php

class TimePeriod {
	public $Seconds = 3600;
	
	public final $Hours {
		get;
		set;
	}
}

class TimePeriod2 extends TimePeriod {
	public $Hours {
		get;
	}
}

$o = new TimePeriod2();

echo $o->Hours."\n";
?>
--EXPECTF--
Fatal error: Cannot override final accessor TimePeriod::$Hours->get() in %s on line %d
