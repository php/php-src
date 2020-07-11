--TEST--
Testing imagefilltoborder() of GD library
--CREDITS--
Ivan Rosolen <contato [at] ivanrosolen [dot] com>
#testfest PHPSP on 2009-06-30
--SKIPIF--
<?php
if (!extension_loaded("gd")) die("skip GD not present");
?>
--FILE--
<?php
// Create a image
$image = imagecreatetruecolor( 100, 100 );

// Draw a rectangle
imagefilledrectangle( $image, 0, 0, 100, 100, imagecolorallocate( $image, 255, 255, 255 ) );

// Draw an ellipse to fill with a black border
imageellipse( $image, 50, 50, 50, 50, imagecolorallocate( $image, 0, 0, 0 ) );

// Fill border
imagefilltoborder( $image, 50, 50, imagecolorallocate( $image, 0, 0, 0 ), imagecolorallocate( $image, 255, 0, 0 ) );

include_once __DIR__ . '/func.inc';
test_image_equals_file(__DIR__ . '/imagefilltoborder_basic.png', $image);

?>
--EXPECT--
The images are equal.
