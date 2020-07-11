--TEST--
Bug #77200 (imagecropauto(…, GD_CROP_SIDES) crops left but not right)
--SKIPIF--
<?php
if (!extension_loaded('gd')) die('skip gd extension not available');
if (!GD_BUNDLED) die('skip upstream bugfix has not been released');
?>
--FILE--
<?php

$orig = imagecreatetruecolor(8, 8);
$red = imagecolorallocate($orig, 255, 0, 0);
$green = imagecolorallocate($orig, 0, 255, 0);
$blue = imagecolorallocate($orig, 0, 0, 255);

imagefilledrectangle($orig, 0, 0, 3, 3, $green); // tl
imagefilledrectangle($orig, 4, 0, 7, 3, $red);   // tr
imagefilledrectangle($orig, 0, 4, 3, 7, $green); // bl
imagefilledrectangle($orig, 4, 4, 7, 7, $blue);  // br

$cropped = imagecropauto($orig, IMG_CROP_SIDES);
var_dump(imagesx($cropped));

imagefilledrectangle($orig, 0, 0, 3, 3, $red);   // tl
imagefilledrectangle($orig, 4, 0, 7, 3, $green); // tr
imagefilledrectangle($orig, 0, 4, 3, 7, $blue);  // bl
imagefilledrectangle($orig, 4, 4, 7, 7, $green); // br

$cropped = imagecropauto($orig, IMG_CROP_SIDES);
var_dump(imagesx($cropped));

?>
--EXPECT--
int(4)
int(4)
