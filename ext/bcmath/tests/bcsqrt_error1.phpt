--TEST--
bcsqrt — Get the square root of an arbitrary precision number
--CREDITS--
Antoni Torrents
antoni@solucionsinternet.com
--SKIPIF--
<?php if(!extension_loaded("bcmath")) print "skip"; ?>
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
