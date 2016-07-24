--TEST--
imagecreatefrombmp() - basic functionality
--SKIPIF--
<?php
if (!extension_loaded('gd')) die('skip ext/gd required');
if (!(imagetypes() & IMG_BMP)) die('skip BMP support required');
?>
--FILE--
<?php
// create an image from a BMP file
$im = imagecreatefrombmp(__DIR__ . '/imagecreatefrombmp_basic.bmp');

// write the md5 hash of its PNG representation
ob_start();
imagepng($im);
echo md5(ob_get_clean());
?>
--EXPECT--
c0ba2c301a9e36bba57bbf1cc517f9b6
