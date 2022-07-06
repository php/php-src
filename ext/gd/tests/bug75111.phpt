--TEST--
Bug #75111 (Memory disclosure or DoS via crafted .bmp image)
--SKIPIF--
<?php
if (!extension_loaded('gd')) die('skip gd extension not available');
?>
--FILE--
<?php
// craft BMP image
$str  = hex2bin("424D3603000000000000");
$str .= pack("V", -0x120000);   // offset of image data
$str .= pack("V", 40);          // length of header
$str .= pack("V", 256);         // width
$str .= pack("V", 256);         // height
$str .= hex2bin("01001800000000000000000000000000000000000000000000000000");

var_dump(imagecreatefromstring($str));
?>
--EXPECTF--
Warning: imagecreatefromstring(): Passed data is not in "BMP" format in %s on line %d

Warning: imagecreatefromstring(): Couldn't create GD Image Stream out of Data in %s on line %d
bool(false)
