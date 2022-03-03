--TEST--
Bug #72227: imagescale out-of-bounds read
--EXTENSIONS--
gd
--FILE--
<?php

$img = imagecreatetruecolor ( 100, 100);
imagescale($img, 13, 1, IMG_BICUBIC);
?>
DONE
--EXPECT--
DONE
