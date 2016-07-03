--TEST--
Bug #67447 (imagecrop() adds a black line when cropping)
--FILE--
<?php
// true color
$image = imagecreatetruecolor(500, 500);
$red = imagecolorallocate($image, 255, 0, 0);
imagefill($image, 0, 0, $red);
$cropped = imagecrop($image, ['x' => 0, 'y' => 0, 'width' => 250, 'height' => 250]);
var_dump(imagecolorat($cropped, 249, 249) === $red);
imagedestroy($image);
imagedestroy($cropped);

// palette
$image = imagecreate(500, 500);
imagecolorallocate($image, 0, 0, 255); // first palette color = background
$red = imagecolorallocate($image, 255, 0, 0);
imagefill($image, 0, 0, $red);
$cropped = imagecrop($image, ['x' => 0, 'y' => 0, 'width' => 250, 'height' => 250]);
var_dump(imagecolorat($cropped, 249, 249) === $red);
imagedestroy($image);
imagedestroy($cropped);
?>
--EXPECT--
bool(true)
bool(true)
