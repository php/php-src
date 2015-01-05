--TEST-- 
Testing wrong param passing imagefilltoborder() of GD library 
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

// Try to fill border
imagefilltoborder( 'wrong param', 50, 50, imagecolorallocate( $image, 0, 0, 0 ), imagecolorallocate( $image, 255, 0, 0 ) );

?> 
--EXPECTF--
Warning: imagefilltoborder() expects parameter 1 to be resource, %s given in %s on line %d
