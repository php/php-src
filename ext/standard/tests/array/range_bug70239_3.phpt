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
--EXPECTF--
The supplied range exceeds the maximum array size: start=-%d end=0
