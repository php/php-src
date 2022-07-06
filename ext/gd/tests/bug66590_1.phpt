--TEST--
Bug #66590 (imagewebp() doesn't pad to even length) - segfault
--SKIPIF--
<?php
if (!extension_loaded('gd')) die('skip gd extension not available');
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
