--TEST--
GH-17384: number_format() must reject decimals outside the int range
--SKIPIF--
<?php
if (PHP_INT_SIZE !== 8) die('skip only for 64-bit');
?>
--FILE--
<?php

foreach ([1.23456, 1] as $number) {
    foreach ([9876543210, -9876543210] as $decimals) {
        try {
            number_format($number, $decimals);
        } catch (ValueError $exception) {
            echo $exception->getMessage(), "\n";
        }
    }
}

?>
--EXPECT--
number_format(): Argument #2 ($decimals) must be between -2147483648 and 2147483647
number_format(): Argument #2 ($decimals) must be between -2147483648 and 2147483647
number_format(): Argument #2 ($decimals) must be between -2147483648 and 2147483647
number_format(): Argument #2 ($decimals) must be between -2147483648 and 2147483647
