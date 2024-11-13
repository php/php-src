--TEST--
GH-16257 (underflow on RBG channels handling with imagescale)
--EXTENSIONS--
gd
--FILE--
<?php
$im = imagecreatefromstring(file_get_contents(__DIR__ . '/imagecreatefromstring.gif'));
imagescale($im, 32, 32, IMG_BICUBIC);
echo "DONE";
?>
--EXPECT--
DONE
