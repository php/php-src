--TEST--
Bug #77479 (imagewbmp() segfaults with very large image)
--EXTENSIONS--
gd
--SKIPIF--
<?php
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
--EXPECTF--
Warning: imagewbmp():%S %croduct of memory allocation multiplication would exceed INT_MAX, failing operation gracefully
 in %s on line %d

Warning: imagewbmp(): Could not create WBMP%win %s on line %d
--CLEAN--
<?php
@unlink(__DIR__ . '/77479.wbmp');
?>
