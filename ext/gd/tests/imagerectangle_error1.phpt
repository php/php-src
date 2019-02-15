--TEST--
Testing wrong param passing imagerectangle() of GD library
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
imagerectangle( 'wrong param', 0, 0, 50, 50, imagecolorallocate($image, 255, 255, 255) );
?>
--EXPECTF--
Warning: imagerectangle() expects parameter 1 to be resource, %s given in %s on line %d
