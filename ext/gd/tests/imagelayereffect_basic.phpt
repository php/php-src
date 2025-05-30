--TEST--
Testing imagelayereffect() of GD library
--CREDITS--
Rafael Dohms <rdohms [at] gmail [dot] com>
#testfest PHPSP on 2009-06-20
--EXTENSIONS--
gd
--SKIPIF--
<?php
    if (!GD_BUNDLED) die('skip function only available in bundled, external GD detected');
?>
--FILE--
<?php
$image = imagecreatetruecolor(180, 30);

imagelayereffect($image, IMG_EFFECT_REPLACE);

include_once __DIR__ . '/func.inc';
test_image_equals_file(__DIR__ . '/imagelayereffect_basic.png', $image);
?>
--EXPECT--
The images are equal.
