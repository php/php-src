--TEST--
Bug #72298: pass2_no_dither out-of-bounds access
--SKIPIF--
<?php
    if (!extension_loaded('gd')) die("skip gd extension not available\n");
?>
--FILE--
<?php
$img = imagecreatetruecolor (1 , 1);
imagecolortransparent($img, 0);
imagetruecolortopalette($img, false, 4);
?>
DONE
--EXPECT--
DONE
