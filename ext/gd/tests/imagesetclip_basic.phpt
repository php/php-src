--TEST--
imagesetclip() - basic functionality
--EXTENSIONS--
gd
--FILE--
<?php
// draw a clipped diagonal line
$im = imagecreate(100, 100);
imagecolorallocate($im, 0, 0, 0);
$white = imagecolorallocate($im, 255, 255, 255);
imagesetclip($im, 10,10, 89,89);
imageline($im, 0,0, 99,99, $white);

// save image for manual inspection
// imagepng($im, __FILE__ . '.png');

// verify that the clipping has been respected
imagesetclip($im, 0,0, 99,99);
var_dump(imagecolorat($im, 9,9) !== $white);
var_dump(imagecolorat($im, 90,90) !== $white);
?>
--EXPECT--
bool(true)
bool(true)
