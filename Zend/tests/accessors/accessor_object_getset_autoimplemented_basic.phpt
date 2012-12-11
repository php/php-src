--TEST--
ZE2 Tests that an autoimplemented getter/setter works properly
--FILE--
<?php

class AccessorTest {
	public $b {
		get;
		set;
	}
}

$o = new AccessorTest();

echo "\$o->b = ".$o->b.PHP_EOL;
$o->b = 1;
echo "\$o->b = ".$o->b.PHP_EOL;

?>
--EXPECTF--
$o->b = 
$o->b = 1
