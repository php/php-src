--TEST--
Creating a range that exceeds the maximum array size
--FILE--
<?php
try {
    var_dump(range(0, 100_000_000_000, 0.1));
} catch (\ValueError $e) {
    echo $e->getMessage(), \PHP_EOL;
}
try {
    var_dump(range(PHP_INT_MIN, PHP_INT_MAX, 1));
} catch (\ValueError $e) {
    echo $e->getMessage(), \PHP_EOL;
}
?>
--EXPECTF--
The supplied range exceeds the maximum array size by %f elements: start=0.0, end=%f, step=0.1. Max size: %d
The supplied range exceeds the maximum array size by 18446744072635809792 elements: start=-9223372036854775808, end=9223372036854775807, step=1. Calculated size: 18446744073709551615. Maximum size: 1073741824.
