--TEST--
SPL: FixedArray: Trying to access nonexistent item
--FILE--
<?php

try {
	$a = new SplFixedArray(NULL);
	echo $a[0]++;
} catch (\ValueError $e) {
	echo $e->getMessage();
}

?>
--EXPECT--
Index invalid or out of range
