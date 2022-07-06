--TEST--
Testing wrong array size 2x3 in imageconvolution() of GD library
--CREDITS--
Guilherme Blanco <guilhermeblanco [at] hotmail [dot] com>
#testfest PHPSP on 2009-06-20
--SKIPIF--
<?php
if (!extension_loaded("gd")) die("skip GD not present");
?>
--FILE--
<?php
require __DIR__ . '/func.inc';

$image = imagecreatetruecolor(180, 30);

// Writes the text and apply a gaussian blur on the image
imagestring($image, 5, 10, 8, 'Gaussian Blur Text', 0x00ff00);

$gaussian = array(
    array(1.0, 2.0, 1.0),
    array(2.0, 4.0, 2.0)
);

trycatch_dump(
    fn() => imageconvolution($image, $gaussian, 16, 0)
);

?>
--EXPECT--
!! [ValueError] imageconvolution(): Argument #2 ($matrix) must be a 3x3 array
