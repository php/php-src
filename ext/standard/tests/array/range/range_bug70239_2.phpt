--TEST--
Bug #70239 Creating a huge array doesn't result in exhausted, but segfault, var 3
--FILE--
<?php
try {
    var_dump(range(0, PHP_INT_MAX));
} catch (\ValueError $e) {
    echo $e->getMessage() . "\n";
}
?>
--EXPECT--
The supplied range exceeds the maximum array size by 9223372035781033984 elements: start=0, end=9223372036854775807, step=1. Calculated size: 9223372036854775807. Maximum size: 1073741824.
