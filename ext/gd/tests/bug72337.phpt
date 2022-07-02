--TEST--
 #72337	segfault in imagescale with new dimensions being <=0)
--EXTENSIONS--
gd
--FILE--
<?php
$im = imagecreatetruecolor(1, 1);
imagescale($im, 0, 0, IMG_BICUBIC_FIXED);
echo "OK";
?>
--EXPECT--
OK
