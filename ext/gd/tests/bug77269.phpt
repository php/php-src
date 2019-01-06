--TEST--
Bug #77269 (Potential unsigned underflow in gdImageScale)
--SKIPIF--
<?php
if (!extension_loaded('gd')) die('skip gd extension not available');
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
?>
--INI--
memory_limit=2G
--FILE--
<?php
$im = imagecreate(2**28, 1);
if(is_resource($im)) {
	imagescale($im, 1, 1, IMG_TRIANGLE);
}
?>
===DONE===
--EXPECTF--
Warning: imagescale():%S product of memory allocation multiplication would exceed INT_MAX, failing operation gracefully
 in %s on line %d
===DONE===
