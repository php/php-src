--TEST--
Testing error with non-double first paramenter of imagegammacorrect() of GD library, 
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
$gamma = imagegammacorrect($image, 'string', 5);

?>
--EXPECTF--
Warning: imagegammacorrect() expects parameter 2 to be double, %s given in %s on line %d
