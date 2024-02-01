--TEST--
Testing imagefilledarc() of GD library
--CREDITS--
Edgar Ferreira da Silva <contato [at] edgarfs [dot] com [dot] br>
#testfest PHPSP on 2009-06-20
--EXTENSIONS--
gd
--SKIPIF--
<?php
if (!GD_BUNDLED && version_compare(GD_VERSION, '2.2.2', '<')) {
    die("skip test requires GD 2.2.2 or higher");
}
if (!(imagetypes() & IMG_PNG)) {
    die("skip No PNG support");
}
?>
--FILE--
<?php

$image = imagecreatetruecolor(100, 100);

$white = imagecolorallocate($image, 0xFF, 0xFF, 0xFF);

//create an arc and fill it with white color
imagefilledarc($image, 50, 50, 30, 30, 0, 90, $white, IMG_ARC_PIE);

include_once __DIR__ . '/func.inc';
test_image_equals_file(__DIR__ . '/imagefilledarc_basic.png', $image);
?>
--EXPECT--
The images are equal.
