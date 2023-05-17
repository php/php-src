--TEST--
Bug #39780 (PNG image with CRC/data error raises a fatal error)
--EXTENSIONS--
gd
--SKIPIF--
<?php
    if (GD_BUNDLED) die("skip requires extern GD\n");
?>
--FILE--
<?php

$im = imagecreatefrompng(__DIR__ . '/bug39780.png');
var_dump($im);
?>
--EXPECTF--
Warning: imagecreatefrompng(): gd-png: fatal libpng error: Read Error: truncated data%win %s on line %d

Warning: imagecreatefrompng(): gd-png error: setjmp returns error condition %d%win %s on line %d

Warning: imagecreatefrompng(): "%sbug39780.png" is not a valid PNG file in %s on line %d
bool(false)
