--TEST--
SPL: FixedArray: adding new elements
--FILE--
<?php

$a = new SplFixedArray(10);

try {
	$a[] = 1;
} catch (\ValueError $e) {
    echo $e->getMessage() . PHP_EOL;
}

?>
--EXPECT--
Index invalid or out of range
