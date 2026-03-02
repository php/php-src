--TEST--
Testing null byte injection in imagegd
--EXTENSIONS--
gd
--FILE--
<?php
$image = imagecreate(1,1);// 1px image
try {
    imagegd($image, "./foo\0bar");
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
imagegd(): Argument #2 ($file) must not contain any null bytes
