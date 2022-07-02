--TEST--
Test imagecolorstotal() function : basic functionality
--CREDITS--
Felix De Vliegher <felix.devliegher@gmail.com>
--EXTENSIONS--
gd
--SKIPIF--
<?php
    if (!function_exists("imagecolorstotal")) {
        die("skip imagecolorstotal() not available.");
    }
?>
--FILE--
<?php
echo "*** Testing imagecolorstotal() : basic functionality ***\n";

// Palette image
$img = imagecreate( 50, 50 );
var_dump( imagecolorstotal( $img ) );
$bg = imagecolorallocate( $img, 255, 255, 255 );
var_dump( imagecolorstotal( $img ));
$bg = imagecolorallocate( $img, 255, 0, 0 );
$bg = imagecolorallocate( $img, 0, 0, 255 );
var_dump( imagecolorstotal( $img ));
imagedestroy( $img );

// Truecolor image
$img = imagecreatetruecolor( 50, 50 );
var_dump( imagecolorstotal( $img ) );
$bg = imagecolorallocate( $img, 255, 255, 255 );
var_dump( imagecolorstotal( $img ) );
imagedestroy( $img );

?>
--EXPECT--
*** Testing imagecolorstotal() : basic functionality ***
int(0)
int(1)
int(3)
int(0)
int(0)
