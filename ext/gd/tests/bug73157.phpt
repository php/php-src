--TEST--
Bug #73157 (imagegd2() ignores 3rd param if 4 are given)
--SKIPIF--
<?php
if (!extension_loaded('gd')) die('skip gd extension not available');
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
