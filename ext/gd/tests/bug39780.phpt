--TEST--
Bug #39780 (PNG image with CRC/data error raises a fatal error)
--SKIPIF--
<?php 
	if (!extension_loaded('gd')) die("skip gd extension not available\n"); 
	if (!GD_BUNDLED) die('skip external GD libraries always fail');
?>
--FILE--
<?php

$im = imagecreatefrompng(dirname(__FILE__) . '/bug39780.png');
var_dump($im);
?>
--EXPECTF--

Warning: imagecreatefrompng(): gd-png:  fatal libpng error: Read Error: truncated data in /home/pierre/cvs/php_5_2/ext/gd/tests/bug39780.php on line 3

Warning: imagecreatefrompng(): gd-png error: setjmp returns error condition in /home/pierre/cvs/php_5_2/ext/gd/tests/bug39780.php on line 3

Warning: imagecreatefrompng(): '/home/pierre/cvs/php_5_2/ext/gd/tests/bug39780.png' is not a valid PNG file in /home/pierre/cvs/php_5_2/ext/gd/tests/bug39780.php on line 3
bool(false)
