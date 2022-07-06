--TEST--
Testing imagecreatetruecolor(): error on out of bound parameters
--CREDITS--
Rafael Dohms <rdohms [at] gmail [dot] com>
--SKIPIF--
<?php
    if (!extension_loaded("gd")) die("skip GD not present");
    if (!function_exists("imagecreatetruecolor")) die("skip GD Version not compatible");
?>
--FILE--
<?php

require __DIR__ . '/func.inc';

trycatch_dump(
    fn() => imagecreatetruecolor(-1, 30),
    fn() => imagecreatetruecolor(30, -1)
);

?>
--EXPECT--
!! [ValueError] imagecreatetruecolor(): Argument #1 ($width) must be greater than 0
!! [ValueError] imagecreatetruecolor(): Argument #2 ($height) must be greater than 0
