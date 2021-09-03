--TEST--
Test imagecolorstotal() function : basic functionality
--EXTENSIONS--
gd
--SKIPIF--
<?php
if(!function_exists('imagecolorstotal') || !function_exists('imagecreatefromgif')) {
    die('skip imagecolorstotal and imagecreatefromgif functions not available in this build');
}
?>
--FILE--
<?php
echo "*** Testing imagecolorstotal() : basic functionality ***\n";

// Get an image
$gif = __DIR__."/php.gif";
$im = imagecreatefromgif($gif);

echo 'Total colors in image: ' . imagecolorstotal($im);

// Free image
imagedestroy($im);
?>
--EXPECT--
*** Testing imagecolorstotal() : basic functionality ***
Total colors in image: 128
