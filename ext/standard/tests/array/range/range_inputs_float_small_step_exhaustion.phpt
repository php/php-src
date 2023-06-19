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
The supplied range exceeds the maximum array size: start=0.0 end=100000000000.0 step=0.1
The supplied range exceeds the maximum array size: start=-%d end=%d step=1
