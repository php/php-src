--TEST--
Bug #53640 (XBM images require width to be multiple of 8)
--SKIPIF--
<?php
if (!extension_loaded('gd')) die('skip gd extension not available');
?>
--FILE--
<?php
$im = imagecreate(9, 9);
imagecolorallocate($im, 0, 0, 0); // background
$white = imagecolorallocate($im, 255, 255, 255);
imagefilledrectangle($im, 2, 2, 6, 6, $white);
imagexbm($im, NULL);
?>
--EXPECT--
#define image_width 9
#define image_height 9
static unsigned char image_bits[] = {
  0xFF, 0x01, 0xFF, 0x01, 0x83, 0x01, 0x83, 0x01, 0x83, 0x01, 0x83, 0x01, 
  0x83, 0x01, 0xFF, 0x01, 0xFF, 0x01};
