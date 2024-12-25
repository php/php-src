--TEST--
Bitmaps with bpp < 8
--EXTENSIONS--
gd
--FILE--
<?php
require __DIR__ . "/func.inc";

$filename = __DIR__ . "/bmp_low_depth.bmp";

// 2 color image
$im1 = imagecreate(99, 99); // odd pixels per row!
imagecolorallocate($im1, 0, 0, 0); // background
$fg = imagecolorallocate($im1, 255, 255, 255);
imagerectangle($im1, 0, 0, 98, 98, $fg); // border to verify edge case
imageellipse($im1, 49, 49, 75, 75, $fg);

echo "1bpp uncompressed\n";
imagebmp($im1, $filename, false);
$data = file_get_contents($filename, false, null, 0x1c, 6);
var_dump(unpack("vbpp/Vcomp", $data));
$im2 = imagecreatefrombmp($filename);
test_image_equals_image($im1, $im2);

echo "\n1bpp compressed\n";
imagebmp($im1, $filename, true);
$data = file_get_contents($filename, false, null, 0x1c, 6);
var_dump(unpack("vbpp/Vcomp", $data));
$im2 = imagecreatefrombmp($filename);
test_image_equals_image($im1, $im2);

// 5 color image
$r = imagecolorallocate($im1, 255, 0, 0);
$g = imagecolorallocate($im1, 0, 255, 0);
$b = imagecolorallocate($im1, 0, 0, 255);
imageellipse($im1, 17, 17, 28, 28, $r);
imageellipse($im1, 49, 17, 28, 28, $r);
imageellipse($im1, 81, 17, 28, 28, $r);
imageellipse($im1, 17, 49, 28, 28, $g);
imageellipse($im1, 49, 49, 28, 28, $g);
imageellipse($im1, 81, 49, 28, 28, $g);
imageellipse($im1, 17, 81, 28, 28, $b);
imageellipse($im1, 49, 81, 28, 28, $b);
imageellipse($im1, 81, 81, 28, 28, $b);

echo "\n4bpp uncompressed\n";
imagebmp($im1, $filename, false);
$data = file_get_contents($filename, false, null, 0x1c, 6);
var_dump(unpack("vbpp/Vcomp", $data));
$im2 = imagecreatefrombmp($filename);
test_image_equals_image($im1, $im2);

echo "\n4bpp compressed\n";
imagebmp($im1, $filename, true);
$data = file_get_contents($filename, false, null, 0x1c, 6);
var_dump(unpack("vbpp/Vcomp", $data));
$im2 = imagecreatefrombmp($filename);
test_image_equals_image($im1, $im2);
?>
--EXPECT--
1bpp uncompressed
array(2) {
  ["bpp"]=>
  int(1)
  ["comp"]=>
  int(0)
}
The images are equal.

1bpp compressed
array(2) {
  ["bpp"]=>
  int(4)
  ["comp"]=>
  int(2)
}
The images are equal.

4bpp uncompressed
array(2) {
  ["bpp"]=>
  int(4)
  ["comp"]=>
  int(0)
}
The images are equal.

4bpp compressed
array(2) {
  ["bpp"]=>
  int(4)
  ["comp"]=>
  int(2)
}
The images are equal.
--CLEAN--
<?php
@unlink(__DIR__ . "/bmp_low_depth.bmp");
?>
