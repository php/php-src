--TEST--
libgd #86 (Possible infinite loop in imagecreatefrompng)
--SKIPIF--
<?php
	if (!extension_loaded('gd')) die("skip gd extension not available\n");
	if (GD_BUNDLED) die("skip requires external GD library\n");
?>
--FILE--
<?php

$im = imagecreatefrompng(__DIR__ . '/libgd00086.png');
var_dump($im);
?>
--EXPECTF--
gd-png:  fatal libpng error: Read Error: truncated data
gd-png error: setjmp returns error condition 1

Warning: imagecreatefrompng(): '%slibgd00086.png' is not a valid PNG file in %s on line %d
bool(false)
