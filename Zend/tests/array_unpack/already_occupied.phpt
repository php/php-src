--TEST--
Appending to an array via unpack may fail
--SKIPIF--
<?php if (PHP_INT_SIZE != 8) die("skip 64bit only"); ?>
--FILE--
<?php

$arr = [1, 2, 3];
try {
    var_dump([PHP_INT_MAX-1 => 0, ...$arr]);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

try {
    var_dump([PHP_INT_MAX-1 => 0, ...[1, 2, 3]]);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

const ARR = [1, 2, 3];
function test($x = [PHP_INT_MAX-1 => 0, ...ARR]) {}
try {
    test();
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
Cannot add element to the array as the next element is already occupied
Cannot add element to the array as the next element is already occupied
Cannot add element to the array as the next element is already occupied
