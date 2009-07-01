--TEST--
Testing imageellipse() of GD library
--CREDITS--
Ivan Rosolen <contato [at] ivanrosolen [dot] com>
#testfest PHPSP on 2009-06-20
--SKIPIF--
<?php 
if (!extension_loaded("gd")) die("skip GD not present");
?>
--FILE--
<?php

// Create a image
$image = imagecreatetruecolor(400, 300);

// Draw a white ellipse
imageellipse($image, 200, 150, 300, 200, 16777215);

ob_start();
imagepng($image, null, 9);
$img = ob_get_contents();
ob_end_clean();

echo md5(base64_encode($img));
?>
--EXPECT--
d8b9bc2ca224bd68569413f4617f8e1f
