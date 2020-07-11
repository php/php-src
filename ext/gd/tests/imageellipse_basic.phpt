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

include_once __DIR__ . '/func.inc';
test_image_equals_file(__DIR__ . '/imageellipse_basic.png', $image);
?>
--EXPECT--
The images are equal.
