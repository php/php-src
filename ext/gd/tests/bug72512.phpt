--TEST--
Bug #19366 (gdimagefill() function crashes (fixed in bundled libgd))
--SKIPIF--
<?php
    if (!extension_loaded('gd')) die("skip gd extension not available\n");
    if (!GD_BUNDLED && version_compare(GD_VERSION, '2.2.2', '<')) {
        die("skip test requires GD 2.2.2 or higher");
    }
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
