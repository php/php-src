--TEST--
imagecreatefrombmp() doesn't reach hard limit
--EXTENSIONS--
gd
--SKIPIF--
<?php
if (!GD_BUNDLED) die("skip requires bundled GD library");
if (!function_exists("imagecreatefrombmp")) die("skip BMP support unavailable");
?>
--INI--
memory_limit=64M
--FILE--
<?php
$w = $h = 20000;
$bmp = "BM" . pack('V', 0) . pack('v', 0) . pack('v', 0) . pack('V', 54)
     . pack('V', 40) . pack('V', $w) . pack('V', $h) . pack('v', 1) . pack('v', 24)
     . pack('V', 0) . pack('V', 0) . pack('V', 0) . pack('V', 0) . pack('V', 0) . pack('V', 0);

$path = __DIR__ . '/' . basename(__FILE__, '.php') . '.tmp.bmp';
file_put_contents($path, $bmp);
imagecreatefrombmp($path);
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '.tmp.bmp');
?>
--EXPECTF--
Fatal error: Allowed memory size of %d bytes exhausted %S(tried to allocate %d bytes) in %s on line %d
