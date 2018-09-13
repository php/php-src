--TEST--
image2wbmp() is deprecated
--SKIPIF--
<?php
if (!extension_loaded('gd')) die('skip gd extension not available');
?>
--FILE--
<?php
$im = imagecreate(10, 10);
imagecolorallocate($im, 0, 0, 0);
image2wbmp($im, __DIR__ . '/image2wbmp_error.wbmp');
?>
===DONE===
--CLEAN--
<?php
unlink(__DIR__ . '/image2wbmp_error.wbmp');
?>
--EXPECTF--
Deprecated: Function image2wbmp() is deprecated in %s on line %d
===DONE===
