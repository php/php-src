--TEST--
json_encode() rejects depth values outside the encoder range
--SKIPIF--
<?php if (PHP_INT_SIZE != 8) die("skip this test is for 64bit platform only"); ?>
--FILE--
<?php

foreach ([2147483648, -2147483649] as $depth) {
    try {
        json_encode([], depth: $depth);
    } catch (ValueError $e) {
        echo $e->getMessage(), "\n";
    }
}

?>
--EXPECT--
json_encode(): Argument #3 ($depth) must be between -2147483648 and 2147483647
json_encode(): Argument #3 ($depth) must be between -2147483648 and 2147483647
