--TEST--
SPL: FastArray: Trying to access inexistent item
--FILE--
<?php

try {
	$a = new SplFastArray(NULL);
	echo $a[0]++;
} catch (Exception $e) {
	echo $e->getMessage();
}

?>
--EXPECT--
Index invalid or out of range
