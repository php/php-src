--TEST--
Testing null byte injection in imagegd2
--SKIPIF--
<?php
        if(!extension_loaded('gd')){ die('skip gd extension not available'); }
?>
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
