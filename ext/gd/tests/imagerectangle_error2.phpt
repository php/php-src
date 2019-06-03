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
// Create a resource
$image = tmpfile();

// Draw a rectangle
try {
    imagerectangle( $image, 0, 0, 50, 50, 2 );
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
imagerectangle(): supplied resource is not a valid Image resource
