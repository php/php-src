--TEST--
GH-17384: number_format() must reject too large decimals
--SKIPIF--
<?php
if (PHP_INT_SIZE !== 8) die('skip only for 64-bit');
?>
--FILE--
<?php

foreach ([1.23456, 1] as $number) {
    try {
        number_format($number, 9876543210);
    } catch (ValueError $exception) {
        echo $exception->getMessage(), "\n";
    }
}

var_dump(number_format(1.23456, -9876543210));

?>
--EXPECT--
number_format(): Argument #2 ($decimals) must be less than or equal to 2147483647
number_format(): Argument #2 ($decimals) must be less than or equal to 2147483647
string(1) "0"
