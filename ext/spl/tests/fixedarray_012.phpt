--TEST--
SPL: FixedArray: Assigning the object to another variable using []
--FILE--
<?php

$a = new SplFixedArray(100);

try {
	$b = &$a[];
} catch (\ValueError $e) {
	echo $e->getMessage(), "\n";
}

?>
--EXPECT--
Index invalid or out of range
