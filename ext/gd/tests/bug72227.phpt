--TEST--
Bug #72227: imagescale out-of-bounds read
--SKIPIF--
<?php
    if (!extension_loaded('gd')) die("skip gd extension not available\n");
?>
--FILE--
<?php

$img = imagecreatetruecolor ( 100, 100);
imagescale($img, 13, 1, IMG_BICUBIC);
?>
DONE
--EXPECT--
DONE
