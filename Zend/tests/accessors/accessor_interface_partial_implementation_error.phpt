--TEST--
ZE2 Tests that an interface which declares a getter/setter errors if an implementor does not implement one of them
--FILE--
<?php

interface Hours {
	public $Hours { get; set; }
}

class TimePeriod implements Hours {
	public $Seconds = 3600;
	
	public $Hours {
		get;
	}
}

?>
--EXPECTF--
Fatal error: Class TimePeriod contains 2 abstract accessors and must be declared abstract or implement the remaining accessors (Hours::$Hours->set, Hours::$Hours->unset) in %s on line %d
