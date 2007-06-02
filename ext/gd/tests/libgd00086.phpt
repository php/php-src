--TEST--
libgd #86 (Possible infinite loop in imagecreatefrompng)
--SKIPIF--
<?php
	if (!extension_loaded('gd')) die("skip gd extension not available\n");
	if (!GD_BUNDLED) die("skip requires bundled GD library\n");
?>
--FILE--
<?php

$im = imagecreatefrompng(dirname(__FILE__) . '/libgd00086.png');
var_dump($im);
?>
--EXPECTF--

Warning: imagecreatefrompng(): gd-png:  fatal libpng error: Read Error: truncated data in %s on line %d

Warning: imagecreatefrompng(): gd-png error: setjmp returns error condition in %s on line %d

Warning: imagecreatefrompng(): '%s' is not a valid PNG file in %s on line %d
bool(false)
