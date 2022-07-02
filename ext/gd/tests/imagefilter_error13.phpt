--TEST--
Testing wrong parameter value of COLORIZE in imagefilter() of GD library
--CREDITS--
Guilherme Blanco <guilhermeblanco [at] hotmail [dot] com>
#testfest PHPSP on 2009-06-20
--EXTENSIONS--
gd
--FILE--
<?php
$image = imagecreatetruecolor(180, 30);

var_dump(imagefilter($image, IMG_FILTER_COLORIZE, 800, 255, 255)); // Wrong value is truncated to 255
?>
--EXPECT--
bool(true)
