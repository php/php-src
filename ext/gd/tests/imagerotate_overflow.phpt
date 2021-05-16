--TEST--
imagerotate() overflow with negative numbers
--EXTENSIONS--
gd
--SKIPIF--
<?php
    if (!function_exists('imagerotate')) {
        die("skip imagerotate() not available.");
    }
?>
--FILE--
<?php

$im = imagecreate(10, 10);

$tmp = imagerotate ($im, 5, -9999999);

var_dump($tmp);

if ($tmp) {
        imagedestroy($tmp);
}

if ($im) {
        imagedestroy($im);
}

?>
--EXPECT--
bool(false)
