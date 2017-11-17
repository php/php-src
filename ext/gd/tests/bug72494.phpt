--TEST--
Bug #72494 (imagecropauto out-of-bounds access)
--SKIPIF--
<?php
if (!extension_loaded('gd')) die('skip gd extension not available');
?>
--FILE--
<?php
$im = imagecreate(10,10);
imagecropauto($im, IMG_CROP_THRESHOLD, 0, 1337);
?>
===DONE===
--EXPECTF--
Warning: imagecropauto(): Color argument missing with threshold mode in %s on line %d
===DONE===
