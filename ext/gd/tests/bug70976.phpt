--TEST--
Bug #70976 (Memory Read via gdImageRotateInterpolated Array Index Out of Bounds)
--EXTENSIONS--
gd
--FILE--
<?php
$img = imagerotate(imagecreate(10,10),45,0x7ffffff9);
var_dump($img);
?>
--EXPECT--
object(GdImage)#2 (0) {
}
