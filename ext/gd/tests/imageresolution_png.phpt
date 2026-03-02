--TEST--
Set and get image resolution of PNG images
--EXTENSIONS--
gd
--SKIPIF--
<?php
if (!(imagetypes() & IMG_PNG)) {
    die("skip No PNG support");
}
?>
--FILE--
<?php
$filename = __DIR__ . DIRECTORY_SEPARATOR . 'imageresolution_png.png';

$exp = imagecreate(100, 100);
imagecolorallocate($exp, 255, 0, 0);

imageresolution($exp, 71);
imagepng($exp, $filename);
$act = imagecreatefrompng($filename);
var_dump(imageresolution($act));

imageresolution($exp, 71, 299);
imagepng($exp, $filename);
$act = imagecreatefrompng($filename);
var_dump(imageresolution($act));
?>
--EXPECT--
array(2) {
  [0]=>
  int(71)
  [1]=>
  int(71)
}
array(2) {
  [0]=>
  int(71)
  [1]=>
  int(299)
}
--CLEAN--
<?php
@unlink(__DIR__ . DIRECTORY_SEPARATOR . 'imageresolution_png.png');
?>
