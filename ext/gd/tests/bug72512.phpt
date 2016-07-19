--TEST--
Bug #19366 (gdimagefill() function crashes (fixed in bundled libgd))
--SKIPIF--
<?php
	if (!extension_loaded('gd')) die("skip gd extension not available\n");
?>
--FILE--
<?php
$img = imagecreatetruecolor(100, 100);
imagecolortransparent($img, -1000000);
imagetruecolortopalette($img, TRUE, 3);
imagecolortransparent($img, 9);
echo "OK";
?>
--EXPECT--
OK

