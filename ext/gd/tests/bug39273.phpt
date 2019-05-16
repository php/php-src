--TEST--
Bug #37360 (gdimagecreatefromgif, bad image sizes)
--SKIPIF--
<?php
	if (!extension_loaded('gd')) die("skip gd extension not available\n");
?>
--FILE--
<?php
$small = imagecreatetruecolor(10, 10);
$c1 = imagecolorallocatealpha($small, 255,0,0,50);
imagecolortransparent($small, 0);
imagealphablending($small, 0);
imagefilledrectangle($small, 0,0, 6,6, $c1);

$width = 300;
$height = 300;
$srcw = imagesx($small);
$srch = imagesy($small);

$img = imagecreatetruecolor($width, $height);

imagecolortransparent($img, 0);
imagealphablending($img, false);
imagecopyresized($img, $small, 0,0, 0,0, $width, $height, $srcw, $srch);
imagesavealpha($img, 1);

$c = imagecolorat($img, 0,0);
printf("%X", $c);
?>
--EXPECT--
32FF0000
