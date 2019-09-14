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
!! [ValueError] Invalid width (x_size)
!! [ValueError] Invalid height (y_size)
