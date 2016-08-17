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

ob_start(); 
imagegd( $image );
$img = ob_get_contents(); 
ob_end_clean();

echo md5(base64_encode($img));

?> 
--EXPECT-- 
e7f8ca8c63fb08b248f3ed6435983aed