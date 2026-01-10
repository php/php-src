--TEST--
Testing imagearc() of GD library
--CREDITS--
Edgar Ferreira da Silva <contato [at] edgarfs [dot] com [dot] br>
#testfest PHPSP on 2009-06-20
--EXTENSIONS--
gd
--SKIPIF--
<?php
if (!(imagetypes() & IMG_PNG)) {
    die("skip No PNG support");
}
?>
--FILE--
<?php

$image = imagecreatetruecolor(100, 100);

$white = imagecolorallocate($image, 0xFF, 0xFF, 0xFF);

//create an arc with white color
imagearc($image, 50, 50, 30, 30, 0, 180, $white);

include_once __DIR__ . '/func.inc';
test_image_equals_file(__DIR__ . '/imagearc_basic.png', $image);
?>
--EXPECT--
The images are equal.
