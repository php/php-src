--TEST--
Bug #72339 Integer Overflow in _gd2GetHeader() resulting in heap overflow 
--SKIPIF--
<?php if (!function_exists("imagecreatefromgd2")) print "skip"; ?>
--FILE--
<?php imagecreatefromgd2(dirname(__FILE__) . DIRECTORY_SEPARATOR . "bug72339.gd"); ?>
--EXPECTF--	
Warning: imagecreatefromgd2(): gd warning: product of memory allocation multiplication would exceed INT_MAX, failing operation gracefully
 in %sbug72339.php on line %d

Warning: imagecreatefromgd2(): '%sbug72339.gd' is not a valid GD2 file in %sbug72339.php on line %d
