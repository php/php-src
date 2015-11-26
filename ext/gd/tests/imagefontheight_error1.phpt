--TEST--
Testing error on string parameter for imagefontheight() of GD library
--CREDITS--
Rafael Dohms <rdohms [at] gmail [dot] com>
#testfest PHPSP on 2009-06-20
--SKIPIF--
<?php 
	if (!extension_loaded("gd")) die("skip GD not present");
?>
--FILE--
<?php
imagefontheight('string');
?>
--EXPECTF--
Warning: imagefontheight() expects parameter 1 to be integer, %s given in %s on line %d
