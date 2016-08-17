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

ob_start(); 
imagegd( $image);
$img = ob_get_contents(); 
ob_end_clean();

echo md5(base64_encode($img));

?> 
--EXPECT-- 
8185a06ccff03c2abeb99d5e3ed60e45
