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
$image = imagecreatetruecolor(150, 150);

$a = imagecolorallocate($image,255,0,255);
$b = imagecolorallocate($image,0,255,255);

$half =  imagefilledarc ( $image, 75, 75, 70, 70, 0, 180, $a, IMG_ARC_PIE );
$half2 =  imagefilledarc ( $image, 75, 55, 80, 70, 0, -180, $b, IMG_ARC_PIE );

var_dump(imagetruecolortopalette($image, true, 2));

ob_start();
imagegd($image);
$img = ob_get_contents();
ob_end_clean();

echo md5(base64_encode($img));
?>
--EXPECT--
bool(true)
17c901cb2217eb7647deed1c79d445de