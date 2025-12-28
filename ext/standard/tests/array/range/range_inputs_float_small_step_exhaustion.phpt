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
The supplied range exceeds the maximum array size by %d elements: start=-%d, end=%d, step=1. Calculated size: %d. Maximum size: %d.
