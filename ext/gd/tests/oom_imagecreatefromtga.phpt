--TEST--
imagecreatefromtga() doesn't reach hard limit
--EXTENSIONS--
gd
--SKIPIF--
<?php
if (!GD_BUNDLED) die("skip requires bundled GD library");
if (!function_exists("imagecreatefromtga")) die("skip TGA support unavailable");
?>
--INI--
memory_limit=64M
--FILE--
<?php
// 8192 x 8192 x 4 bytes per pixel x sizeof(int) = 1 GB.
$tga = pack('CCCvvCvvvvCC',
    0,      // id length
    0,      // colour map type
    2,      // image type: uncompressed true colour
    0, 0, 0,// colour map spec
    0, 0,   // x/y origin
    8192,   // width
    8192,   // height
    32,     // bits per pixel
    8       // image descriptor
) . str_repeat("\x00", 64);

$path = __DIR__ . '/oom_imagecreatefromtga.tmp.tga';
file_put_contents($path, $tga);
imagecreatefromtga($path);
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/oom_imagecreatefromtga.tmp.tga');
?>
--EXPECTF--
Fatal error: Allowed memory size of %d bytes exhausted %S(tried to allocate %d bytes) in %s on line %d
