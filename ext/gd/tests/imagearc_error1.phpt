--TEST--
Testing wrong param passing imagearc() of GD library
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

//create an arc with white color    
imagearc($image, 50, 50, 30, 30, 0, 180);

include_once __DIR__ . '/func.inc';
test_image_equals_file(__DIR__ . '/imagearc_error1.png', $image);
?>
--EXPECTF--
Warning: imagearc() expects exactly 8 parameters, 7 given in %s on line %d
The images are equal.
