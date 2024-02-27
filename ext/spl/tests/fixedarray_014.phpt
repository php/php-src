--TEST--
SPL: FixedArray: Trying to access inexistent item
--FILE--
<?php

$a = new SplFixedArray(0);
try {
    echo $a[0]++;
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
OutOfBoundsException: Index invalid or out of range
