--TEST--
Testing error on non-long parameter 6 of imagechar() of GD library
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
$result = imagechar($image, 1, 5, 5, 'C', 'font');

?>
--EXPECTF--
Warning: imagechar() expects parameter 6 to be integer, %s given in %s on line %d
