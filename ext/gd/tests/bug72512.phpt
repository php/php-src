--TEST--
Bug #19366 (gdimagefill() function crashes (fixed in bundled libgd))
--EXTENSIONS--
gd
--SKIPIF--
<?php
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
