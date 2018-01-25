--TEST--
Testing wrong param passing imagefilledarc() of GD library
--CREDITS--
Edgar Ferreira da Silva <contato [at] edgarfs [dot] com [dot] br>
#testfest PHPSP on 2009-06-20
--SKIPIF--
<?php
if (!extension_loaded("gd")) die("skip GD not present");
?>
--FILE--
<?php

$image = imagecreatetruecolor(100, 100);

$white = imagecolorallocate($image, 0xFF, 0xFF, 0xFF);

//create an arc and fill it with white color
imagefilledarc($image, 50, 50, 30, 30, 0, 90, $white);

include_once __DIR__ . '/func.inc';
test_image_equals_file(__DIR__ . '/imagefilledarc_error1.png', $image);
?>
--EXPECTF--
Warning: imagefilledarc() expects exactly 9 parameters, 8 given in %s on line %d
The images are equal.
