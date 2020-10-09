--TEST--
Testing null byte injection in imagejpeg
--SKIPIF--
<?php
if(!extension_loaded('gd')){ die('skip gd extension not available'); }
$support = gd_info();
if (!isset($support['JPEG Support']) || $support['JPEG Support'] === false) {
    print 'skip jpeg support not available';
}
?>
--FILE--
<?php
$image = imagecreate(1,1);// 1px image
try {
    imagejpeg($image, "./foo\0bar");
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
imagejpeg(): Argument #2 ($file) must not contain null bytes
