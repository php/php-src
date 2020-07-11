--TEST--
does the imagepalettetotruecollor function really converts the image palette?
--CREDITS--
Carlos André Ferrari <caferrari [at] gmail [dot] com>
--SKIPIF--
<?php
    if (!extension_loaded('gd')) die("skip gd extension not available.");
?>
--FILE--
<?php
$im = imagecreate(100, 100);
var_dump($im instanceof GdImage);
var_dump(imageistruecolor($im));
var_dump(imagepalettetotruecolor($im));
var_dump(imageistruecolor($im));
imagedestroy($im);
?>
--EXPECT--
bool(true)
bool(false)
bool(true)
bool(true)
