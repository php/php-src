--TEST--
ZE2 Tests that an interface which declares a getter/setter can be legally implemented by defining a traditional property
--FILE--
<?php

interface Hours {
	public $Hours { get; set; }
}

class TimePeriod implements Hours {
	public $Hours;
}

$o = new TimePeriod();
$o->Hours = 5;
echo $o->Hours.PHP_EOL
?>
===DONE===
--EXPECTF--
5
===DONE===
