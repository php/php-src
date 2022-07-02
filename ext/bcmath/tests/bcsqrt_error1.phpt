--TEST--
bcsqrt — Get the square root of an arbitrary precision number
--CREDITS--
Antoni Torrents
antoni@solucionsinternet.com
--EXTENSIONS--
bcmath
--FILE--
<?php
try {
    bcsqrt('-9');
} catch (ValueError $ex) {
    echo $ex->getMessage(), PHP_EOL;
}
?>
--EXPECT--
bcsqrt(): Argument #1 ($num) must be greater than or equal to 0
