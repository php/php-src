--TEST--
Bug #77269 (Potential unsigned underflow in gdImageScale)
--EXTENSIONS--
gd
--SKIPIF--
<?php
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
if (substr(PHP_OS, 0, 3) == 'WIN' && PHP_INT_SIZE === 4) die("skip not for Windows x86");
?>
--INI--
memory_limit=2G
--FILE--
<?php
$im = imagecreate(2**28, 1);
imagescale($im, 1, 1, IMG_TRIANGLE);

?>
--EXPECTF--
Warning: imagescale():%S Product of memory allocation multiplication would exceed INT_MAX, failing operation gracefully
 in %s on line %d
