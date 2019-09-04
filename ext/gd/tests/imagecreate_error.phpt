--TEST--
Testing imagecreate(): error on out of bound parameters
--SKIPIF--
<?php
	if (!extension_loaded("gd")) die("skip GD not present");
	if (!function_exists("imagecreate")) die("skip GD Version not compatible");
?>
--FILE--
<?php

require __DIR__ . '/func.inc';

trycatch_dump(
    fn() => imagecreate(-1, 30),
    fn() => imagecreate(30, -1)
);

?>
--EXPECT--
!! [ValueError] Invalid width (x_size)
!! [ValueError] Invalid height (y_size)
