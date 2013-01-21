--TEST--
ZE2 Tests that a getter/setter defined in a trait is inherited appropriately
--FILE--
<?php

trait Hours {
	public $Hours {
		get { return $this->Seconds / 3600; }
		set { $this->Seconds = $value * 3600; }
	}
}

class TimePeriod {
	use Hours;
	public $Seconds = 3600;
	
}

$o = new TimePeriod();

echo $o->Hours."\n";
$o->Hours = 2;
echo $o->Hours."\n";
?>
--EXPECTF--
1
2
