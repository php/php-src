--TEST--
Bug #73159 (imagegd2(): unrecognized formats may result in corrupted files)
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
$im = imagecreatetruecolor(10, 10);

ob_start();
imagegd2($im, null, 128, 0);
$buffer = ob_get_clean();

$header = unpack('@12/nformat', $buffer);
printf("format: %d\n", $header['format']);
?>
--EXPECT--
format: 4
