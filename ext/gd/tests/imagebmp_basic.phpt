--TEST--
imagebmp() - basic functionality
--SKIPIF--
<?php
if (!extension_loaded('gd')) die('skip ext/gd required');
if (!(imagetypes() & IMG_BMP)) die('skip BMP support required');
?>
--FILE--
<?php
// create an image
$im = imagecreate(100, 100);
imagecolorallocate($im, 0, 0, 0);
$white = imagecolorallocate($im, 255, 255, 255);
imageline($im, 10,10, 89,89, $white);

// write the md5 hash of its BMP representation
ob_start();
imagebmp($im);
echo md5(ob_get_clean());
?>
--EXPECT--
d49124076771822b09fa72e168c0de56
