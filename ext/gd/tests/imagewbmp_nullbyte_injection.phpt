--TEST--
Testing null byte injection in imagewbmp
--EXTENSIONS--
gd
--SKIPIF--
<?php
$support = gd_info();
if (!isset($support['WBMP Support']) || $support['WBMP Support'] === false) {
    print 'skip wbmp support not available';
}
?>
--FILE--
<?php
$image = imagecreate(1,1);// 1px image
try {
    imagewbmp($image, "./foo\0bar");
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
imagewbmp(): Argument #2 ($file) must not contain null bytes
