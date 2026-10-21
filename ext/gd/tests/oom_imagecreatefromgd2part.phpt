--TEST--
imagecreatefromgd2part() doesn't reach hard limit
--EXTENSIONS--
gd
--SKIPIF--
<?php
if (!GD_BUNDLED) die("skip requires bundled GD library");
if (!function_exists("imagecreatefromgd2part")) die("skip GD2 support unavailable");
?>
--INI--
memory_limit=64M
--FILE--
<?php
$w = fn($v) => pack('n', $v);   // big-endian 16-bit
$gd2 = "gd2\x00"                  // magic
     . $w(2)                      // version
     . $w(1) . $w(1)             // image dimensions (small)
     . $w(64)                     // chunk size
     . $w(2)                      // GD2_FMT_COMPRESSED
     . $w(65535) . $w(512);      // ncx, ncy -> 33.5M chunk-index entries

$path = __DIR__ . '/' . basename(__FILE__, '.php') . '.tmp.gd2';
file_put_contents($path, $gd2);
imagecreatefromgd2part($path, 0, 0, 10, 10);
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '.tmp.gd2');
?>
--EXPECTF--
Fatal error: Allowed memory size of %d bytes exhausted %S(tried to allocate %d bytes) in %s on line %d
