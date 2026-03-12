--TEST--
Bug #69024 (imagescale segfault with palette based image)
--EXTENSIONS--
gd
--FILE--
<?php
$im = imagecreate(256, 256);
imagescale($im, 32, 32, IMG_BICUBIC);
echo "done\n";
?>
--EXPECT--
done
