--TEST--
imagecreatefromgif() doesn't reach hard limit
--EXTENSIONS--
gd
--SKIPIF--
<?php
if (!GD_BUNDLED) die("skip requires bundled GD library");
if (!function_exists("imagecreatefromgif")) die("skip GIF support unavailable");
?>
--INI--
memory_limit=64M
--FILE--
<?php
$w = $h = 20000;
$gif = "GIF89a" . pack('v', $w) . pack('v', $h) . "\x00\x00\x00"       // screen descriptor, no GCT
     . "," . pack('v', 0) . pack('v', 0) . pack('v', $w) . pack('v', $h) . "\x00"  // image descriptor
     . ";";

$path = __DIR__ . '/' . basename(__FILE__, '.php') . '.tmp.gif';
file_put_contents($path, $gif);
imagecreatefromgif($path);
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '.tmp.gif');
?>
--EXPECTF--
Fatal error: Allowed memory size of %d bytes exhausted %S(tried to allocate %d bytes) in %s on line %d
