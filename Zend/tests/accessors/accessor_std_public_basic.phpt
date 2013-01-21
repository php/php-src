--TEST--
ZE2 Tests that a getter/setter defined as public applies to both getter/setter
--FILE--
<?php

class TimePeriod {
	public $Seconds = 3600;
	
	public $Hours {
		get { return $this->Seconds / 3600; }
		set { $this->Seconds = $value * 3600; }
	}
}

$o = new TimePeriod();

echo $o->Hours."\n";
$o->Hours = 2;
echo $o->Seconds."\n";

echo "Done\n";
?>
--EXPECTF--
1
7200
Done