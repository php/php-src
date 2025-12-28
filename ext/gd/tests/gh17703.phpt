--TEST--
GH-17703 both width and height value being negative triggers ValueError on width.
--EXTENSIONS--
gd
--FILE--
<?php

$img = imagecreatetruecolor ( 256, 1);

try {
	imagescale($img, -1, -1, 0);
} catch (\ValueError $e) {
	echo $e->getMessage();
}
?>
--EXPECT--
Argument #2 ($width) and argument #3 ($height) cannot be both negative
