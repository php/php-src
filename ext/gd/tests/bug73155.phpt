--TEST--
Bug #73155 (imagegd2() writes wrong chunk sizes on boundaries)
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
$im = imagecreate(64, 64);
imagecolorallocate($im, 0, 0, 0);

ob_start();
imagegd2($im, null, 64, IMG_GD2_RAW);
$buffer = ob_get_clean();

$header = unpack('@10/nchunk_size/nformat/nx_count/ny_count', $buffer);
printf("chunk size: %d\n", $header['chunk_size']);
printf("x chunk count: %d\n", $header['x_count']);
printf("y chunk count: %d\n", $header['y_count']);
printf("file size: %d\n", strlen($buffer));
?>
--EXPECT--
chunk size: 64
x chunk count: 1
y chunk count: 1
file size: 5145
