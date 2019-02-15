--TEST--
Testing wrong param passing imageellipse() of GD library
--CREDITS--
Ivan Rosolen <contato [at] ivanrosolen [dot] com>
#testfest PHPSP on 2009-06-20
--SKIPIF--
<?php
if ( ! extension_loaded("gd") ) die( 'skip GD not present; skipping test' );
?>
--FILE--
<?php

// Create a image
$image = imagecreatetruecolor( 400, 300 );

// try to draw a white ellipse
imageellipse( $image, 200, 150, 300, 200 );

?>
--EXPECTF--
Warning: imageellipse() expects exactly 6 parameters, %d given in %s on line %d
