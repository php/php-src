--TEST--
Testing error with non-double second paramenter of imagegammacorrect() of GD library,
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
$gamma = imagegammacorrect($image, 1, 'string');

?>
--EXPECTF--
Warning: imagegammacorrect() expects parameter 3 to be float, %s given in %s on line %d
