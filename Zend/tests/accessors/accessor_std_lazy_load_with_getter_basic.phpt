--TEST--
ZE2 Tests that a getter may define its own property for use with lazy-loading via a getter
	This perpetuates a side-effect of the way __get() works, which allows you to define the gotten property
		which circumvents calling __get() again.
--FILE--
<?php

class TimePeriod {
	public $Seconds = 3600;
	
	public $Hours {
		get {
			echo "Get Hours Called\n";
			return $this->Hours = 1;	/* Calls Setter */
		}
		set {
			$this->Hours = 1;			/* Sets an anonymous property due to guard allowing lazy load */ 
		}
	}
}

$o = new TimePeriod();

echo $o->Hours."\n";
echo $o->Hours."\n";
$o->Hours = 4;
?>
--EXPECTF--
Get Hours Called
1
1
