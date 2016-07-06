--TEST--
Bug #72512 gdImageTrueColorToPaletteBody allows arbitrary write/read access, var 0
--SKIPIF--
<?php
	if (!extension_loaded('gd')) die("skip gd extension not available\n");
?>
--FILE--
<?php

$img = imagecreatetruecolor(13, 1007);

imagecolortransparent($img, -10066304);
imagetruecolortopalette($img, TRUE, 3);
imagescale($img, 1, 65535);
?>
==DONE==
--EXPECT--
==DONE==
