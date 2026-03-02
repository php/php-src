--TEST--
libgd #94 (imagecreatefromxbm can crash if gdImageCreate fails)
--EXTENSIONS--
gd
--SKIPIF--
<?php
    if (!GD_BUNDLED) die("skip requires bundled GD library\n");
?>
--FILE--
<?php
$im = imagecreatefromxbm(__DIR__ . '/libgd00094私はガラスを食べられます.xbm');
var_dump($im);
?>
--EXPECTF--
Warning: imagecreatefromxbm(): "%slibgd00094私はガラスを食べられます.xbm" is not a valid XBM file in %slibgd00094-mb.php on line %d
bool(false)
