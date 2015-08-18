--TEST--
Testing error on non-long parameter 2 of imagechar() of GD library
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
$result = imagechar($image, 'string', 5, 5, 'C', 1);

?>
--EXPECTF--
Warning: imagechar() expects parameter 2 to be integer, %s given in %s on line %d
