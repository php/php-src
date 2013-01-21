--TEST--
ZE2 Tests that a getter can be defined with or without parenthesis
--FILE--
<?php

class TimePeriod {
	public $Seconds = 3600;
	
	public $Hours {
		get { return $this->Seconds / 3600; }
	}
	public $Hours2 {
		get;
	}
	public $Hours3 {
		get();
	}
	public $Hours4 {
		get() { return $this->Seconds / 3600; }
	}
}

$o = new TimePeriod();

echo $o->Hours."\n";
echo $o->Hours2."\n";
echo $o->Hours3."\n";
echo $o->Hours4."\n";
echo $o->Seconds."\n";

echo "Done\n";
?>
--EXPECTF--
1


1
3600
Done