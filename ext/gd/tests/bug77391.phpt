--TEST--
Bug #77391 (1bpp BMPs may fail to be loaded)
--SKIPIF--
<?php
if (!extension_loaded('gd')) die('skip gd extension not available');
if (!GD_BUNDLED && version_compare(GD_VERSION, '2.2.5', '<=')) die('skip upstream not yet fixed');
?>
--FILE--
<?php
var_dump(imagecreatefrombmp(__DIR__ . '/bug77391.bmp'));
?>
--EXPECT--
object(GdImage)#1 (0) {
}
