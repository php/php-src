--TEST--
imagecreatefrompng() doesn't reach hard limit
--EXTENSIONS--
gd
--SKIPIF--
<?php
if (!GD_BUNDLED) die("skip requires bundled GD library");
if (!function_exists("imagecreatefrompng")) die("skip PNG support unavailable");
?>
--INI--
memory_limit=64M
--FILE--
<?php
function png_chunk(string $t, string $d): string {
    return pack('N', strlen($d)) . $t . $d . pack('N', crc32($t . $d));
}
$w = $h = 20000;
$png = "\x89PNG\r\n\x1a\n"
     . png_chunk('IHDR', pack('NNCCCCC', $w, $h, 8, 2, 0, 0, 0))  // 8-bit truecolor
     . png_chunk('IDAT', "\x78\x9c\x63\x00\x00\x00\x01\x00\x01")
     . png_chunk('IEND', '');

$path = __DIR__ . '/' . basename(__FILE__, '.php') . '.tmp.png';
file_put_contents($path, $png);
imagecreatefrompng($path);
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '.tmp.png');
?>
--EXPECTF--
Fatal error: Allowed memory size of %d bytes exhausted %S(tried to allocate %d bytes) in %s on line %d
