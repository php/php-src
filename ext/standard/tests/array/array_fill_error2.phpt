--TEST--
Test array_fill() function : error conditions - count is too large
--SKIPIF--
<?php if (PHP_INT_SIZE != 8) die("skip this test is for 64bit platform only"); ?>
--FILE--
<?php
$intMax = 2147483647;

// calling array_fill() with 'count' larger than INT_MAX
try {
    $array = array_fill(0, $intMax+1, 1);
} catch (\ValueError $e) {
    echo $e->getMessage() . "\n";
}

// calling array_fill() with 'count' equals to INT_MAX
$array = array_fill(0, $intMax, 1);

?>
--EXPECTF--
array_fill(): Argument #2 ($count) is too large

Fatal error: Possible integer overflow in memory allocation (%d * %d + %d) in %s on line %d
