--TEST--
SPL: FixedArray: adding new elements
--FILE--
<?php

$a = new SplFixedArray(10);

try {
	$a[] = 1;
} catch (\Error $e) {
    echo $e->getMessage() . PHP_EOL;
}

?>
--EXPECT--
Dynamic allocation is forbidden
