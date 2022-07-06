--TEST--
Bug #39780 (PNG image with CRC/data error raises a fatal error)
--SKIPIF--
<?php
    if (!extension_loaded('gd')) die("skip gd extension not available\n");
    if (GD_BUNDLED) die("skip requires extern GD\n");
?>
--FILE--
<?php

$im = imagecreatefrompng(__DIR__ . '/bug39780.png');
var_dump($im);
?>
--EXPECTF--
gd-png:  fatal libpng error: Read Error: truncated data
gd-png error: setjmp returns error condition 2
Warning: imagecreatefrompng(): v%sbug39780.png" is not a valid PNG file in /%s on line %d
bool(false)
