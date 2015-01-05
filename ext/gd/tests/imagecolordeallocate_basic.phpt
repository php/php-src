--TEST--
Testing imagecolordeallocate() of GD library
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
$result = imagecolordeallocate($image, $white);

var_dump($result);

?>
--EXPECT--
bool(true)
