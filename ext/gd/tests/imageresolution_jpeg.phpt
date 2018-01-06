--TEST--
Set and get image resolution of JPEG images
--SKIPIF--
<?php
if (!extension_loaded('gd')) die('skip gd extension not available');
if (!(imagetypes() & IMG_JPEG)) die('skip JPEG support not available');
?>
--FILE--
<?php
$filename = __DIR__ . DIRECTORY_SEPARATOR . 'imageresolution_jpeg.jpeg';

$exp = imagecreate(100, 100);
imagecolorallocate($exp, 255, 0, 0);

imageresolution($exp, 71);
imagejpeg($exp, $filename);
$act = imagecreatefromjpeg($filename);
var_dump(imageresolution($act));

imageresolution($exp, 71, 299);
imagejpeg($exp, $filename);
$act = imagecreatefromjpeg($filename);
var_dump(imageresolution($act));
?>
===DONE===
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
===DONE===
--CLEAN--
<?php
@unlink(__DIR__ . DIRECTORY_SEPARATOR . 'imageresolution_jpeg.jpeg');
?>
