--TEST--
Checks that GdImage instances can be cloned from gd > 2.2.3
--EXTENSIONS--
gd
--SKIPIF--
<?php if (version_compare(GD_VERSION, '2.2.3', '<')) die("Skipped: GdImage is not clonable before gd 2.2.3"); ?>
--FILE--
<?php

$foo = imagecreatetruecolor(32, 32);
$bar = clone $foo;

$red = imagecolorallocate($foo, 255, 0, 0);
imagefill($foo, 0, 0, $red);

$green = imagecolorallocate($bar, 0, 255, 0);
imagefill($bar, 0, 0, $green);

var_dump(imagecolorat($foo, 0, 0) === $red);
var_dump(imagecolorat($bar, 0, 0) === $green);


?>
--EXPECTF--
bool(true)
bool(true)
