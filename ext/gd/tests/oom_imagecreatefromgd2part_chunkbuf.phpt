--TEST--
imagecreatefromgd2part() chunk buffer doesn't reach hard limit
--EXTENSIONS--
gd
--SKIPIF--
<?php
if (!GD_BUNDLED) die("skip requires bundled GD library");
if (!function_exists("imagecreatefromgd2part")) die("skip GD2 support unavailable");
?>
--INI--
memory_limit=8M
--FILE--
<?php
$n = fn($v) => pack('n', $v);   // big-endian 16-bit
$N = fn($v) => pack('N', $v);   // big-endian 32-bit

// chunkBuf is sized cs * bytesPerPixel * cs, all of it from the header, so a
// 1x1 image still asks for 4096 * 4 * 4096 = 64 MB. GD2_CHUNKSIZE_MAX caps the
// chunk size at 4096, hence the small memory_limit.
$gd2 = "gd2\0"                  // magic
     . $n(2)                    // version
     . $n(1) . $n(1)            // image dimensions
     . $n(4096)                 // chunk size
     . $n(4)                    // GD2_FMT_TRUECOLOR_COMPRESSED
     . $n(1) . $n(1)            // ncx, ncy -> one chunk
     . $N(31) . $N(16)          // chunk index: offset, compressed size
     . "\x01" . $N(-1);
// The chunk data itself is never reached: the buffers are allocated first.

$path = __DIR__ . '/oom_imagecreatefromgd2part_chunkbuf.tmp.gd2';
file_put_contents($path, $gd2);
imagecreatefromgd2part($path, 0, 0, 1, 1);
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/oom_imagecreatefromgd2part_chunkbuf.tmp.gd2');
?>
--EXPECTF--
Fatal error: Allowed memory size of %d bytes exhausted %S(tried to allocate %d bytes) in %s on line %d
