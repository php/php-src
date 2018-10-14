--TEST--
Testing error on non-long parameter 6 of imagecharup() of GD library
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
$result = imagecharup($image, 1, 5, 5, 'C', 'font');

?>
--EXPECTF--
Warning: imagecharup() expects parameter 6 to be int, %s given in %s on line %d
