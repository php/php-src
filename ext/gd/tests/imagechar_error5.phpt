--TEST--
Testing error on non-long parameter 4 of imagechar() of GD library
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
$result = imagechar($image, 1, 5, 'string', 'C', 1);

?>
--EXPECTF--
Warning: imagechar() expects parameter 4 to be long, %s given in %s on line %d
