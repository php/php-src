--TEST--
ZE2 Tests that a getter/setter defined as private is overridable in a looser manner and that a getter is inherited
--FILE--
<?php

class TimePeriod {
	public $Seconds {
		get;
		set;
	}
}

$o = new TimePeriod();

echo "get Seconds :".$o->Seconds."\n";
echo "Setting Seconds to 3600\n";
$o->Seconds = 3600;
echo "get Seconds :".$o->Seconds."\n";
?>
--EXPECTF--
get Seconds :
Setting Seconds to 3600
get Seconds :3600
