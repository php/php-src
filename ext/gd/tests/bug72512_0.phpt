--TEST--
Bug #72512 gdImageTrueColorToPaletteBody allows arbitrary write/read access, var 0
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

$img = imagecreatetruecolor(13, 1007);

imagecolortransparent($img, -10066304);
imagetruecolortopalette($img, TRUE, 3);
imagescale($img, 1, 65535);
?>
==DONE==
--EXPECT--
==DONE==
