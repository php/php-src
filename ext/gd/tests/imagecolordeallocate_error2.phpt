--TEST--
Testing imagecolordeallocate() of GD library with no resource
--CREDITS--
Rafael Dohms <rdohms [at] gmail [dot] com>
#testfest PHPSP on 2009-06-20
--SKIPIF--
<?php 
	if (!extension_loaded("gd")) die("skip GD not present");
?>
--FILE--
<?php

$image = imagecreatetruecolor(180, 30);
$white = imagecolorallocate($image, 255, 255, 255);
$result = imagecolordeallocate('image', $white);

?>
--EXPECTF--
Warning: imagecolordeallocate() expects parameter 1 to be resource, %s given %s on line %d
