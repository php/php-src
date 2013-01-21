--TEST--
ZE2 Tests that an accessor cannot be declared if a property is already declared
--FILE--
<?php

class TimePeriod {
	public $Seconds;
	public $Seconds {
	}
}

?>
--EXPECTF--
Fatal error: Cannot redeclare TimePeriod::$Seconds in %s on line %d