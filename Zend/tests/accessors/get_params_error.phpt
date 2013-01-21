--TEST--
ZE2 Tests that a getter defined with parameters throws an error
--FILE--
<?php

class TimePeriod {
	public $Hours {
		get($x) { return 5; }
	}
}

?>
--EXPECTF--
Fatal error: Getters do not accept parameters for variable TimePeriod::$Hours in %s on line %d