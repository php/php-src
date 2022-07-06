--TEST--
Bug #37360 (gdimagecreatefromgif, bad image sizes)
--SKIPIF--
<?php
    if (!extension_loaded('gd')) die("skip gd extension not available\n");
?>
--FILE--
<?php
$im = imagecreatefromgif(__DIR__ . '/bug37360.gif');
var_dump($im);
?>
--EXPECTF--
Warning: imagecreatefromgif(): "%s" is not a valid GIF file in %s on line %d
bool(false)
