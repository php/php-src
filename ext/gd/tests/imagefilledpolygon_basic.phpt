--TEST--
imagefilledpolygon()
--SKIPIF--
<?php
    if (!function_exists('imagefilledpolygon')) die('skip imagefilledpolygon() not available');
?>
--FILE--
<?php

echo "Simple test of imagefilledpolygon() function\n";

$dest = dirname(realpath(__FILE__)) . '/imagefilledpolygon.png';

$points = array(
            40,  50,
            20,  240,
            60,  60,
            240, 20,
            50,  40,
            10,  10
            );

// create a blank image
$image = imagecreatetruecolor(250, 250);

// set the background color to black
$bg = imagecolorallocate($image, 0, 0, 0);

// fill polygon with green
$col_poly = imagecolorallocate($image, 0, 255, 0);

// draw the polygon
imagefilledpolygon($image, $points, $col_poly);

// output the picture to a file
imagepng($image, $dest);

// get it back
$image_in = imagecreatefrompng($dest);

//check color of a point on edge..
$col1 = imagecolorat($image_in, 40, 50);
//.. a point in filled body
$col2 = imagecolorat($image_in, 15, 15);
// ..and a point on background
$col3 = imagecolorat($image_in, 5, 5);

$color1 = imagecolorsforindex($image_in, $col1);
$color2 = imagecolorsforindex($image_in, $col2);
$color3 = imagecolorsforindex($image_in, $col3);
var_dump($color1, $color2, $color3);

imagedestroy($image);
imagedestroy($image_in);

echo "Done\n";
?>
--CLEAN--
<?php
    $dest = dirname(realpath(__FILE__)) . '/imagefilledpolygon.png';
    @unlink($dest);
?>
--EXPECT--
Simple test of imagefilledpolygon() function
array(4) {
  ["red"]=>
  int(0)
  ["green"]=>
  int(255)
  ["blue"]=>
  int(0)
  ["alpha"]=>
  int(0)
}
array(4) {
  ["red"]=>
  int(0)
  ["green"]=>
  int(255)
  ["blue"]=>
  int(0)
  ["alpha"]=>
  int(0)
}
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
Done
