--TEST--
GH-19730 (undefined behavior in gd_interpolation.c)
--EXTENSIONS--
gd
--SKIPIF--
<?php
    if (!GD_BUNDLED) {
    	die("skip meaningful only for bundled libgd\n");
    }
?>
--FILE--
<?php
$im = imagecreatetruecolor(8, 8);
$affine = [1211472000, 1, 1, 1, 1, 1];
var_dump(imageaffine($im, $affine));
?>
--EXPECT--
bool(false)
