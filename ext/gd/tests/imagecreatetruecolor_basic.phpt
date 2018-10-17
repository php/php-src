--TEST--
Testing imagecreatetruecolor() of GD library
--CREDITS--
Rafael Dohms <rdohms [at] gmail [dot] com>
--SKIPIF--
<?php
	if (!extension_loaded("gd")) die("skip GD not present");
	if (!function_exists("imagecreatetruecolor")) die("skip GD Version not compatible");
?>
--FILE--
<?php
$image = imagecreatetruecolor(180, 30);

include_once __DIR__ . '/func.inc';
test_image_equals_file(__DIR__ . '/imagecreatetruecolor_basic.png', $image);
?>
--EXPECT--
The images are equal.
