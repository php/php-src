--TEST--
Bug #73157 (imagegd2() ignores 3rd param if 4 are given)
--EXTENSIONS--
gd
--SKIPIF--
<?php
    if (!GD_BUNDLED && version_compare(GD_VERSION, '2.3.3', '>=')) {
        die("skip test requires GD 2.3.2 or older");
    }
?>
--FILE--
<?php
$im = imagecreate(8, 8);
imagecolorallocate($im, 0, 0, 0);

ob_start();
imagegd2($im, null, 256, IMG_GD2_RAW);
$buffer = ob_get_clean();

$header = unpack('@10/nchunk_size', $buffer);
printf("chunk size: %d\n", $header['chunk_size']);
?>
--EXPECT--
chunk size: 256
