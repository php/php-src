--TEST--
imagedashedline()
--SKIPIF--
<?php
    if (!function_exists('imagedashedline')) die('skip imagedashedline() not available');
?>
--FILE--
<?php

echo "Simple test of imagedashedline() function\n";

$dest = dirname(realpath(__FILE__)) . '/imagedashedline.png';

// create a blank image
$image = imagecreatetruecolor(250, 250);

// set the background color to black
$bg = imagecolorallocate($image, 0, 0, 0);

// red dashed lines
$col_line = imagecolorallocate($image, 255, 0, 0);

// draw a couple of vertical dashed lines
imagedashedline($image, 100, 20, 100, 230, $col_line );
imagedashedline($image, 150, 20, 150, 230, $col_line );

// output the picture to a file
imagepng($image, $dest);

//check color of a point on edge..
$col1 = imagecolorat($image, 100, 230);
// ..and a point on background
$col2 = imagecolorat($image, 5, 5);

$color1 = imagecolorsforindex($image, $col1);
$color2 = imagecolorsforindex($image, $col2);
var_dump($color1, $color2);

imagedestroy($image);
echo "Done\n";
?>
--CLEAN--
<?php
    $dest = dirname(realpath(__FILE__)) . '/imagedashedline.png';
    @unlink($dest);
?>
--EXPECT--
Simple test of imagedashedline() function
array(4) {
  ["red"]=>
  int(255)
  ["green"]=>
  int(0)
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
