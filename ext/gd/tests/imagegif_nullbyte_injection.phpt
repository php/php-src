--TEST--
Testing null byte injection in imagegif
--EXTENSIONS--
gd
--FILE--
<?php
$image = imagecreate(1,1);// 1px image
try {
    imagegif($image, "./foo\0bar");
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
imagegif(): Argument #2 ($file) must not contain null bytes
