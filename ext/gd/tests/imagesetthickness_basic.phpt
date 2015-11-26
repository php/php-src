--TEST--
Testing imagetruecolortopalette() of GD library
--CREDITS--
Rafael Dohms <rdohms [at] gmail [dot] com>
--SKIPIF--
<?php 
	if (!extension_loaded("gd")) die("skip GD not present");
	if (!function_exists("imagecreatetruecolor")) die("skip GD Version not compatible");
?>
--FILE--
<?php
// Create a 200x100 image
$image = imagecreatetruecolor(200, 100);
$white = imagecolorallocate($image, 0xFF, 0xFF, 0xFF);
$black = imagecolorallocate($image, 0x00, 0x00, 0x00);

// Set the background to be white
imagefilledrectangle($image, 0, 0, 299, 99, $white);

// Set the line thickness to 5
imagesetthickness($image, 5);

// Draw the rectangle
imagerectangle($image, 14, 14, 185, 85, $black);

ob_start();
imagepng($image, null, 9);
$img = ob_get_contents();
ob_end_clean();

echo md5(base64_encode($img));
?>
--EXPECT--
93c3077f1bdc372cd0b0db96db282985