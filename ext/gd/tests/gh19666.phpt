--TEST--
GH-19666 (Unexpected nan value in imageconvolution)
--EXTENSIONS--
gd
--FILE--
<?php
$image = imagecreatetruecolor(180, 30);

$gaussian = array(
    array(1.0, 2.0, 1.0),
    array(2.0, 4.0, 2.0),
    array(1.0, 2.0, -INF)
);

var_dump(imageconvolution($image, $gaussian, 16, 0));

$rgba = imagecolorat($image, 0, 0);
$colors = imagecolorsforindex($image, $rgba);
var_dump($colors);

imagedestroy($image);
?>
--EXPECT--
bool(true)
array(4) {
  ["red"]=>
  int(0)
  ["green"]=>
  int(0)
  ["blue"]=>
  int(0)
  ["alpha"]=>
  int(0)
}
