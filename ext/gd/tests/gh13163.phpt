--TEST--
GH-13163 (imagecrop on transparent background)
--EXTENSIONS--
gd
--FILE--
<?php
$gdImage = imagecreatetruecolor(100,100);
imagesavealpha($gdImage, true);
imagefill($gdImage, 0, 0, imagecolorallocatealpha($gdImage, 0, 0, 0, 127));
$result = imagecropauto($gdImage, IMG_CROP_TRANSPARENT);
var_dump($result);
--EXPECT--
bool(false)
