--TEST--
Bug #72730: imagegammacorrect allows arbitrary write access
--SKIPIF--
<?php 
if (!function_exists("imagecreatetruecolor")) die("skip");
?>
--FILE--
<?php
$img =  imagecreatetruecolor(1, 1);
imagegammacorrect($img, -1, 1337);
?>
DONE
--EXPECTF--
Warning: imagegammacorrect(): Gamma values should be positive in %sbug72730.php on line %d
DONE