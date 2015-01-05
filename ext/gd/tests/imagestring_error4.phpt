--TEST--
Testing error on non-long parameter 3 of imagestring() of GD library
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
$result = imagestring($image, 1, 'string', 5, 'String', 1);

?>
--EXPECTF--
Warning: imagestring() expects parameter 3 to be integer, %s given in %s on line %d
