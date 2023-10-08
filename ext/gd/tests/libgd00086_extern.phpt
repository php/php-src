--TEST--
libgd #86 (Possible infinite loop in imagecreatefrompng)
--EXTENSIONS--
gd
--SKIPIF--
<?php
    if (GD_BUNDLED) die("skip requires external GD library\n");
?>
--FILE--
<?php

$im = imagecreatefrompng(__DIR__ . '/libgd00086.png');
var_dump($im);
?>
--EXPECTF--
Warning: imagecreatefrompng(): gd-png: fatal libpng error: Read Error: truncated data%win %s on line %d

Warning: imagecreatefrompng(): gd-png error: setjmp returns error condition %d%win %s on line %d

Warning: imagecreatefrompng(): "%slibgd00086.png" is not a valid PNG file in %s on line %d
bool(false)
