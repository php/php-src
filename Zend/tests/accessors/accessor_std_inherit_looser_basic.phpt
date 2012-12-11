--TEST--
ZE2 Tests that a getter/setter defined as private is overridable in a looser manner and that a getter is inherited
--FILE--
<?php

class TimePeriod {
	public $Seconds = 3600;
	
	public $Hours {
		get { return $this->Seconds / 3600; }
		private set { $this->Seconds = $value * 3600; }
	}
}

class TimePeriod2 extends TimePeriod {
	public $Hours {
		public set { $this->Seconds = ($value+1) * 3600; }
	}
}

$o = new TimePeriod2();

echo $o->Hours."\n";
$o->Hours = 2;
echo $o->Hours."\n";
?>
--EXPECTF--
1
3
