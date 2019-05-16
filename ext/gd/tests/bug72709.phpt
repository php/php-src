--TEST--
Bug #72709 (imagesetstyle() causes OOB read for empty $styles)
--SKIPIF--
<?php
if (!extension_loaded('gd')) die('skip ext/gd not available');
?>
--FILE--
<?php
$im = imagecreatetruecolor(1, 1);
var_dump(imagesetstyle($im, array()));
imagesetpixel($im, 0, 0, IMG_COLOR_STYLED);
imagedestroy($im);
?>
====DONE====
--EXPECTF--
Warning: imagesetstyle(): styles array must not be empty in %s%ebug72709.php on line %d
bool(false)
====DONE====
