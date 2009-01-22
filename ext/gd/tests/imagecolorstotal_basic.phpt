--TEST--
Test imagecolorstotal() function : basic functionality
--SKIPIF--
<?php
if(!extension_loaded('gd')) {
    die('skip gd extension is not loaded');
}

if(!function_exists('imagecolorstotal') || !function_exists('imagecreatefromgif')) {
    die('skip imagecolorstotal and imagecreatefromgif functions not available in this build');
}
?> 
--FILE--
<?php
/* Prototype  : int imagecolorstotal  ( resource $image  )
 * Description: Find out the number of colors in an image's palette
 * Source code: ext/gd/gd.c
 */

echo "*** Testing imagecolorstotal() : basic functionality ***\n";

// Get an image 
$gif = dirname(__FILE__)."/php.gif";
$im = imagecreatefromgif($gif);

echo 'Total colors in image: ' . imagecolorstotal($im);

// Free image
imagedestroy($im);
?>

===DONE===
--EXPECTF--
*** Testing imagecolorstotal() : basic functionality ***
Total colors in image: 128
===DONE===