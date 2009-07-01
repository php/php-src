--TEST--
Testing imagecolordeallocate() of GD library with Out of range intergers (Below)
--CREDITS--
Rafael Dohms <rdohms [at] gmail [dot] com>
#testfest PHPSP on 2009-06-20
--SKIPIF--
<?php 
	if (!extension_loaded("gd")) die("skip GD not present");
?>
--FILE--
<?php
$image = imagecreate(180, 30);
$white = imagecolorallocate($image, 255, 255, 255);

$totalColors = imagecolorstotal($image);

$result = imagecolordeallocate($image, -1.0);
var_dump($result);
?>
--EXPECTF--
Warning: imagecolordeallocate(): Color index -1 out of range in %s on line %d
bool(false)
