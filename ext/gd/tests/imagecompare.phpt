--TEST--
imagecompare() function test
--EXTENSIONS--
gd
--SKIPIF--
<?php
if (!(imagetypes() & IMG_PNG)) die("skip No PNG support");
?>
--FILE--
<?php
$SAVE_DIR = __DIR__;
$SOURCE_IMG = $SAVE_DIR . "/test.png";
$im1 = imagecreatefrompng($SOURCE_IMG);
$im2 = imagecreatefrompng($SOURCE_IMG);

var_dump(imagecompare($im1, $im2) === 0);

$cl = imagecolorallocate($im2, 255, 0, 0);
imagecolortransparent($im2, $cl);
var_dump((imagecompare($im1, $im2) & IMG_CMP_TRANSPARENT) === IMG_CMP_TRANSPARENT);

$im3 = imagecreate(10, 10);
imagecopyresized($im3, $im2, 0, 0, 0, 0, 10, 10, 10, 10);
var_dump((imagecompare($im1, $im3) & IMG_CMP_SIZE_X) !== 0);
var_dump((imagecompare($im1, $im3) & IMG_CMP_SIZE_Y) !== 0);
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
