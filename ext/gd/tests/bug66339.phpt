--TEST--
Bug #66339 (PHP segfaults in imagexbm)
--SKIPIF--
<?php
if (!extension_loaded('gd')) die('skip gd extension not available');
?>
--FILE--
<?php
$im = imagecreate(8, 8);
imagecolorallocate($im, 0, 0, 0); // background
$white = imagecolorallocate($im, 255, 255, 255);
imagefilledrectangle($im, 2, 2, 6, 6, $white);
imagexbm($im, NULL);
echo "------------\n";
imagexbm($im, './bug66339.xbm');
echo file_get_contents('./bug66339.xbm');
?>
--CLEAN--
<?php
unlink('./bug66339.xbm');
?>
--EXPECT--
#define image_width 8
#define image_height 8
static unsigned char image_bits[] = {
  0xFF, 0xFF, 0x83, 0x83, 0x83, 0x83, 0x83, 0xFF};
------------
#define bug66339_width 8
#define bug66339_height 8
static unsigned char bug66339_bits[] = {
  0xFF, 0xFF, 0x83, 0x83, 0x83, 0x83, 0x83, 0xFF};
