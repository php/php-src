--TEST--
Testing null byte injection in imagepng
--SKIPIF--
<?php
if(!extension_loaded('gd')){ die('skip gd extension not available'); }
$support = gd_info();
if (!isset($support['PNG Support']) || $support['PNG Support'] === false) {
    print 'skip png support not available';
}
?>
--FILE--
<?php
$image = imagecreate(1,1);// 1px image
try {
    imagepng($image, "./foo\0bar");
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
imagepng(): Argument #2 ($file) must not contain null bytes
