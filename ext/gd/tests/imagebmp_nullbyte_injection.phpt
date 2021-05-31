--TEST--
Testing null byte injection in imagebmp
--EXTENSIONS--
gd
--SKIPIF--
<?php
if (!gd_info()['BMP Support']) die('skip BMP support not available');
?>
--FILE--
<?php
$image = imagecreate(1,1);// 1px image
try {
    imagebmp($image, "./foo\0bar");
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
imagebmp(): Argument #2 ($file) must not contain null bytes
