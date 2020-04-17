--TEST--
SPL: FixedArray: Trying to access nonexistent item
--FILE--
<?php

try {
	$a = new SplFixedArray(1);
	echo $a[0]++;
} catch (\ValueError $e) {
	echo $e->getMessage();
}

?>
--EXPECTF--
Notice: Indirect modification of overloaded element of SplFixedArray has no effect in %s on line %d
