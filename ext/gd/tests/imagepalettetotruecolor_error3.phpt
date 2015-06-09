--TEST--
imagepalettetotruecollor must return an error if not an image resource is given
--CREDITS--
Carlos André Ferrari <caferrari [at] gmail [dot] com>
--SKIPIF--
<?php
    if (!extension_loaded('gd')) die("skip gd extension not available.");
?>
--FILE--
<?php
$im = fopen('php://memory', 'w');
imagepalettetotruecolor($im);
?>
--EXPECTF--
Warning: imagepalettetotruecolor(): supplied resource is not a valid Image resource in %s on line %d
