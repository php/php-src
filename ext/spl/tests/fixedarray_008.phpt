--TEST--
SPL: FixedArray: Assigning the itself object testing the reference
--FILE--
<?php

$b = 3;
$a = new SplFixedArray($b);

$a[0] = 1;
$a[1] = 2;
$a[2] = $a;

$a[2][0] = 3;

foreach ($a as $x) {
	if (is_object($x)) {
		var_dump($x[0]);
	} else {
		var_dump($x);
	}	
}

var_dump($a->getSize());

?>
--EXPECT--
int(3)
int(2)
int(3)
int(3)
