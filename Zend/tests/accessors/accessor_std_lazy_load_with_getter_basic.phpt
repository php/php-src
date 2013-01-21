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
			return $this->Hours ?: $this->Hours = 2;		/* Calls Setter */
		}
		set {
			echo "Set Hours Called ({$value})\n";
			$this->Hours = $value; 
		}
	}
}

$o = new TimePeriod();

echo $o->Hours."\n";
echo $o->Hours."\n";
$o->Hours = 4;
echo $o->Hours."\n";
echo "Done\n";
?>
--EXPECTF--
Get Hours Called
Set Hours Called (2)
2
Get Hours Called
2
Set Hours Called (4)
Get Hours Called
4
Done