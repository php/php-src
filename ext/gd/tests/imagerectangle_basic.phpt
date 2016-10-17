--TEST-- 
Testing imagerectangle() of GD library 
--CREDITS-- 
Ivan Rosolen <contato [at] ivanrosolen [dot] com> 
#testfest PHPSP on 2009-06-30
--SKIPIF-- 
<?php  
if ( ! extension_loaded('gd') ) die( 'skip GD not present; skipping test' ); 
?> 
--FILE--
<?php
// Create a image 
$image = imagecreatetruecolor( 100, 100 ); 

// Draw a rectangle
imagerectangle( $image, 0, 0, 50, 50, imagecolorallocate($image, 255, 255, 255) );

include_once __DIR__ . '/func.inc';
test_image_equals_file(__DIR__ . '/imagerectangle_basic.png', $image);
?> 
--EXPECT-- 
The images are equal.
