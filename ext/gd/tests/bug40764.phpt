--TEST--
Bug #40764 (line thickness not respected for horizontal and vertical lines)
--SKIPIF--
<?php
    if (!extension_loaded('gd')) die("skip gd extension not available\n");
?>
--FILE--
<?php
$image=imagecreatetruecolor(400, 400);
$white = imagecolorallocate($image, 255, 255, 255);
$black = imagecolorallocate($image, 0, 0, 0);
$red = imagecolorallocate($image, 255, 0, 0);

imagefill($image, 0, 0, $white);
imagesetthickness($image, 10);

imageline($image, 200, 0, 200, 400, $black);
imageline($image, 0, 200, 400, 200, $black);
imageline($image, 0, 0, 392, 392, $black);

imagesetthickness($image, 1);

imageline($image, 200, 0, 200, 400, $red);
imageline($image, 0, 200, 400, 200, $red);
imageline($image, 0, 0, 392, 392, $red);
print_r(imagecolorat($image, 195, 0));
print_r(imagecolorat($image, 0, 195));

?>
--EXPECT--
00
