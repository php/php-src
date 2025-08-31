--TEST--
Bug #66590 (imagewebp() doesn't pad to even length) - segfault
--EXTENSIONS--
gd
--SKIPIF--
<?php
if (!function_exists('imagewebp')) die('skip WebP support not available');
?>
--FILE--
<?php
$im = imagecreatetruecolor(6, 6);
ob_start();
imagewebp($im);
ob_end_clean();
echo "ready\n";
?>
--EXPECT--
ready
