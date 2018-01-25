--TEST--
Testing error on string parameter for imagefontwidth() of GD library
--CREDITS--
Rafael Dohms <rdohms [at] gmail [dot] com>
#testfest PHPSP on 2009-06-20
--SKIPIF--
<?php
	if (!extension_loaded("gd")) die("skip GD not present");
?>
--FILE--
<?php
imagefontwidth('string');
?>
--EXPECTF--
Warning: imagefontwidth() expects parameter 1 to be integer, %s given in %s on line %d
