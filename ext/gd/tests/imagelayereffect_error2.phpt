--TEST--
Testing imagelayereffect() wth invalid effect of GD library
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
$image = imagecreatetruecolor(180, 30);

$layer = imagelayereffect($image, 'IMG_EFFECT_REPLACE');
?>
--EXPECTF--
Warning: imagelayereffect() expects parameter 2 to be int, %s given in %s on line %d
