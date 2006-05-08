--TEST--
Bug #37360 (gdimagecreatefromgif, bad image sizes)
--SKIPIF--
<?php 
	if (!extension_loaded('gd')) die("skip gd extension not available\n"); 
	if (!GD_BUNDLED) die('skip external GD libraries always fail');
?>
--FILE--
<?php
$im = imagecreatefromgif(dirname(__FILE__) . '/bug37360.gif');
var_dump($im);
?>
--EXPECTF--
resource(%d) of type (gd)
