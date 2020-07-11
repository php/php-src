--TEST--
Bug #77270 (imagecolormatch Out Of Bounds Write on Heap)
--INI--
memory_limit=-1
--SKIPIF--
<?php
if (!extension_loaded('gd')) die('skip gd extension not available');
if (!GD_BUNDLED && version_compare(GD_VERSION, '2.2.5', '<=')) die('skip upstream bugfix has not been released');
?>
--FILE--
<?php
$img1 = imagecreatetruecolor(0xfff, 0xfff);
$img2 = imagecreate(0xfff, 0xfff);
imagecolorallocate($img2, 0, 0, 0);
imagesetpixel($img2, 0, 0, 255);
imagecolormatch($img1, $img2);
?>
===DONE===
--EXPECT--
===DONE===
