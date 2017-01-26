--TEST--
Testing imagestring() of GD library
--CREDITS--
Rafael Dohms <rdohms [at] gmail [dot] com>
#testfest PHPSP on 2009-06-20
--SKIPIF--
<?php 
	if (!extension_loaded("gd")) die("skip GD not present");
?>
--FILE--
<?php
$image = imagecreatetruecolor(180, 30);
$white = imagecolorallocate($image, 255,255,255);

$result = imagestring($image, 1, 5, 5, 'String Text', $white);

include_once __DIR__ . '/func.inc';
test_image_equals_file(__DIR__ . '/imagestring_basic.png', $image);
?>
--EXPECT--
The images are equal.
