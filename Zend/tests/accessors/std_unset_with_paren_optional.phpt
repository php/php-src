--TEST--
ZE2 Tests that an unsetter may be defined with or without parameters
--FILE--
<?php

class TimePeriod {
	public $Test1 {
		set 	{ }
		unset 	{ }
	}
	public $Test2 {
		set()	{ }
		unset() { }
	}
}

$o = new TimePeriod();

echo "Done\n";
?>
--EXPECTF--
Done