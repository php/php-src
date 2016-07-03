--TEST--
Bug #69024 (imagescale segfault with palette based image)
--SKIPIF--
<?php
if (!extension_loaded('gd')) die('skip gd extension not available');
?>
--FILE--
<?php
$im = imagecreate(256, 256);
imagescale($im, 32, 32, IMG_BICUBIC);
imagedestroy($im);
echo "done\n";
?>
--EXPECT--
done
