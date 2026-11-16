--TEST--
imagecreatefromxbm() doesn't reach hard limit
--EXTENSIONS--
gd
--SKIPIF--
<?php
if (!GD_BUNDLED) die("skip requires bundled GD library");
if (!function_exists("imagecreatefromxbm")) die("skip XBM support unavailable");
?>
--INI--
memory_limit=64M
--FILE--
<?php
$xbm = "#define x_width 20000\n"
     . "#define x_height 20000\n"
     . "static char x_bits[] = {\n0x00 };\n";

$path = __DIR__ . '/' . basename(__FILE__, '.php') . '.tmp.xbm';
file_put_contents($path, $xbm);
imagecreatefromxbm($path);
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '.tmp.xbm');
?>
--EXPECTF--
Fatal error: Allowed memory size of %d bytes exhausted %S(tried to allocate %d bytes) in %s on line %d
