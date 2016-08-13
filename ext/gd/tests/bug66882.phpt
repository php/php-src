--TEST--
Bug #66882 (imagerotate by -90 degrees truncates image by 1px)
--SKIPIF--
<?php
if (!extension_loaded('gd')) die('skip gd extension not available');
?>
--FILE--
<?php
$im = imagerotate(imagecreate(10, 10), -90, 0);
var_dump(imagesy($im), imagesx($im));
?>
--EXPECT--
int(10)
int(10)
