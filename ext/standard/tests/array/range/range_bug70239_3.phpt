--TEST--
Bug #70239 Creating a huge array doesn't result in exhausted, but segfault, var 4
--FILE--
<?php
try {
    var_dump(range(PHP_INT_MIN, 0));
} catch (\ValueError $e) {
    echo $e->getMessage() . "\n";
}
?>
--EXPECT--
The supplied range exceeds the maximum array size by 9223372035781033985 elements: start=-9223372036854775808, end=0, step=1. Calculated size: 9223372036854775808. Maximum size: 1073741824.
