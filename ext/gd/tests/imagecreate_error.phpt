--TEST--
Testing imagecreate(): error on out of bound parameters
--EXTENSIONS--
gd
--SKIPIF--
<?php
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
!! [ValueError] imagecreate(): Argument #1 ($width) must be greater than 0
!! [ValueError] imagecreate(): Argument #2 ($height) must be greater than 0
