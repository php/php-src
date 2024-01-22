--TEST--
Bug #77391 (1bpp BMPs may fail to be loaded)
--EXTENSIONS--
gd
--SKIPIF--
<?php
if (!GD_BUNDLED && version_compare(GD_VERSION, '2.2.5', '<=')) die('skip upstream not yet fixed');
if (!(imagetypes() & IMG_BMP)) die('skip BMP support required');
?>
--FILE--
<?php
var_dump(imagecreatefrombmp(__DIR__ . '/bug77391.bmp'));
?>
--EXPECT--
object(GdImage)#1 (0) {
}
