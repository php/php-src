--TEST--
heif support
--EXTENSIONS--
gd
--SKIPIF--
<?php
    if (!function_exists("imagecreatefromheif") || !function_exists("imageheif")) {
        die("skip HEIF support unavailable");
    }
?>
--FILE--
<?php

    require_once __DIR__ . '/similarity.inc';

    $infile = __DIR__  . '/imageheif_basic.heif';
    $outfile = __DIR__ . '/imageheif_out.heif';

    $img = imagecreatefromheif($infile);
    var_dump($img);
    var_dump(imageheif($img, $outfile));
    unlink($outfile);
?>

--EXPECT--
object(GdImage)#1 (0) {
}
bool(true)
