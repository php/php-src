--TEST--
Bug #77479 (imagewbmp() segfaults with very large image)
--SKIPIF--
<?php
if (!extension_loaded('gd')) die('skip gd extension not available');
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
?>
--INI--
memory_limit=-1
--FILE--
<?php
$im = imagecreate(40000, 20000);
imagecolorallocate($im, 0, 0, 0);
imagewbmp($im, __DIR__ . '/77479.wbmp');
?>
===DONE===
--EXPECTF--
Warning: imagewbmp():%S product of memory allocation multiplication would exceed INT_MAX, failing operation gracefully
 in %s on line %d

Warning: imagewbmp(): Could not create WBMP in %s on line %d
===DONE===
--CLEAN--
<?php
@unlink(__DIR__ . '/77479.wbmp');
?>
