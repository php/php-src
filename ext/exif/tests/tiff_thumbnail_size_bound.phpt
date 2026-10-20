--TEST--
EXIF bounds TIFF thumbnail allocation to the file size
--EXTENSIONS--
exif
--INI--
memory_limit=8M
--FILE--
<?php
$tiff = "II\x2a\x00" . pack('V', 8)
    . pack('v', 1)
    . pack('vvVV', 0x014a, 4, 1, 26)
    . pack('V', 0)
    . pack('v', 2)
    . pack('vvVV', 0x0111, 4, 1, 56)
    . pack('vvVV', 0x0117, 4, 1, 16 * 1024 * 1024)
    . pack('V', 0)
    . 'x';
$file = __DIR__ . '/tiff_thumbnail_size_bound.tmp';
file_put_contents($file, $tiff);

try {
    var_dump(exif_thumbnail($file));
} finally {
    unlink($file);
}
?>
--EXPECTF--
Warning: exif_thumbnail(%s): Thumbnail goes IFD boundary or end of file reached in %s on line %d
bool(false)
