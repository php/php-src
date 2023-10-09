--TEST--
Bug #77272 (imagescale() may return image resource on failure)
--INI--
memory_limit=-1
--EXTENSIONS--
gd
--SKIPIF--
<?php
if (!GD_BUNDLED && version_compare(GD_VERSION, '2.2.5', '<=')) die('skip upstream fix not yet released');
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
if (substr(PHP_OS, 0, 3) == 'WIN' && PHP_INT_SIZE === 4) die("skip not for Windows x86");
?>
--FILE--
<?php
$img = imagecreate(2**28, 1);
var_dump(imagescale($img, 1, 1, IMG_TRIANGLE));
?>
--EXPECTF--
Warning: imagescale():%S %croduct of memory allocation multiplication would exceed INT_MAX, failing operation gracefully
 in %s on line %d
bool(false)
