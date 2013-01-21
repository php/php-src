--TEST--
ZE2 Tests that an issetter may be defined with or without parameters
--FILE--
<?php

class TimePeriod {
	public $Test1 {
		get 	{ return 1; }
		isset 	{ return true; }
	}
	public $Test2 {
		get()	{ return 1; }
		isset() { return false; }
	}
}

$o = new TimePeriod();

echo (int)isset($o->Test1)."\n";
echo (int)isset($o->Test2)."\n";
echo "Done\n";
?>
--EXPECTF--
1
0
Done