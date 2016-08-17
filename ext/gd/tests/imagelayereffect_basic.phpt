--TEST--
Testing imagelayereffect() of GD library
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

$layer = imagelayereffect($image, IMG_EFFECT_REPLACE);

if ($layer){
	ob_start();
	imagegd($image);
	$img = ob_get_contents();
	ob_end_clean();
}

echo md5(base64_encode($img));
?>
--EXPECT--
da884f488a851e0267a316ad232aee86
