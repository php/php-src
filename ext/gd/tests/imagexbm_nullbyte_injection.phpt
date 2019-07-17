--TEST--
Testing null byte injection in imagexbm
--SKIPIF--
<?php
if(!extension_loaded('gd')) die('skip gd extension not available');
?>
--FILE--
<?php
$image = imagecreate(1,1);// 1px image
try {
    imagexbm($image, "./foo\0bar");
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECTF--
imagexbm() expects parameter 2 to be a valid path, string given
