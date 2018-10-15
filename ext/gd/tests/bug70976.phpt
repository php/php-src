--TEST--
Bug #70976 (Memory Read via gdImageRotateInterpolated Array Index Out of Bounds)
--SKIPIF--
<?php
	if(!extension_loaded('gd')){ die('skip gd extension not available'); }
?>
--FILE--
<?php
$img = imagerotate(imagecreate(10,10),45,0x7ffffff9);
var_dump($img);
?>
--EXPECTF--
resource(5) of type (gd)
