--TEST--
ZE2 Tests that an unsetter defined with parameters throws an error
--FILE--
<?php

class TimePeriod {
	public $Hours {
		unset($x) { return 5; }
	}
}

?>
--EXPECTF--
Parse error: syntax error, unexpected '$x' (T_VARIABLE), expecting ')' in %s on line %d