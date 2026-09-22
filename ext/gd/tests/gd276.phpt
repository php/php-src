--TEST--
libgd bug 276 (Sometimes pixels are missing when storing images as BMPs)
--EXTENSIONS--
gd
--FILE--
<?php
require __DIR__ . "/func.inc";

$orig = imagecreate(10, 10);
imagecolorallocate($orig, 0, 0, 0);
$white = imagecolorallocate($orig, 255, 255, 255);
imageline($orig, 0, 0, 99, 99, $white);

$filename = __DIR__ . "/gd276.bmp";
imagebmp($orig, $filename, true);
$saved = imagecreatefrombmp($filename);
var_dump($saved !== false);
imagepalettetotruecolor($orig);
imagepalettetotruecolor($saved);
test_image_equals_image($orig, $saved);
?>
--EXPECT--
bool(true)
The images are equal.
--CLEAN--
<?php
@unlink(__DIR__ . "/gd276.bmp");
?>
