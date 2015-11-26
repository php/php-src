--TEST--
Testing imagelayereffect() with invalid resource of GD library
--CREDITS--
Rafael Dohms <rdohms [at] gmail [dot] com>
#testfest PHPSP on 2009-06-20
--SKIPIF--
<?php 
	if (!extension_loaded("gd")) die("skip GD not present");
	if (!GD_BUNDLED) die('skip function only available in bundled, external GD detected');
?>
--FILE--
<?php
$resource = tmpfile();
$layer = imagelayereffect($resource, IMG_EFFECT_REPLACE);
?>
--EXPECTF--
Warning: imagelayereffect(): supplied resource is not a valid Image resource in %s on line %d
