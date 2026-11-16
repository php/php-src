--TEST--
imagecreatefromgd2() compressed chunk buffer doesn't reach hard limit
--EXTENSIONS--
gd
--SKIPIF--
<?php
if (!GD_BUNDLED) die("skip requires bundled GD library");
if (!function_exists("imagecreatefromgd2")) die("skip GD2 support unavailable");
?>
--INI--
memory_limit=64M
--FILE--
<?php
$n = fn($v) => pack('n', $v);   // big-endian 16-bit
$N = fn($v) => pack('N', $v);   // big-endian 32-bit

// compBuf is sized from the largest "size" in the chunk index, which is only
// checked for being non-negative, so a 1x1 image can ask for 2 GB.
$gd2 = "gd2\0"                   // magic
     . $n(2)                     // version
     . $n(1) . $n(1)             // image dimensions
     . $n(64)                    // chunk size
     . $n(4)                     // GD2_FMT_TRUECOLOR_COMPRESSED
     . $n(1) . $n(1)             // ncx, ncy -> one chunk
     . $N(31) . $N(0x7ffffff0)   // chunk index: offset, compressed size
     . "\x01" . $N(-1);
// The chunk data itself is never reached: the buffers are allocated first.

$path = __DIR__ . '/oom_imagecreatefromgd2_compbuf.tmp.gd2';
file_put_contents($path, $gd2);
imagecreatefromgd2($path);
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/oom_imagecreatefromgd2_compbuf.tmp.gd2');
?>
--EXPECTF--
Fatal error: Allowed memory size of %d bytes exhausted %S(tried to allocate %d bytes) in %s on line %d
