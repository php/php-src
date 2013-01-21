--TEST--
ZE2 Tests that an auto-implemented getter has an auto-implemented variable defined and that it can be retrieved through the accessor
--FILE--
<?php

class AccessorTest {
	public $b {
		get;
	}
}

$o = new AccessorTest();

var_dump($o);

echo "\$o->b: ".$o->b."\n";
echo "Done\n";
?>
--EXPECTF--
object(AccessorTest)#1 (1) {
  ["b"]=>
  NULL
}
$o->b: 
Done