--TEST--
exif_thumbnail() rejects a TIFF thumbnail that is not present in the file
--EXTENSIONS--
exif
--INI--
memory_limit=128M
--FILE--
<?php
// 68 byte TIFF whose thumbnail IFD claims a 4 GiB StripByteCounts.
$ifd1_off = 8 + 2 + 2*12 + 4;
$t  = "II*\x00" . pack("V", 8);
$t .= pack("v", 2);
$t .= pack("vvVV", 0x0100, 4, 1, 1);          // ImageWidth
$t .= pack("vvVV", 0x0101, 4, 1, 1);          // ImageLength
$t .= pack("V", $ifd1_off);                   // next IFD = thumbnail IFD
$t .= pack("v", 2);
$t .= pack("vvVV", 0x0111, 4, 1, 1);          // StripOffsets
$t .= pack("vvVV", 0x0117, 4, 1, -1);         // StripByteCounts = 4 GiB
$t .= pack("V", 0);

$file = __DIR__ . '/oom_exif_thumbnail_tiff.tif';
file_put_contents($file, $t);

var_dump(exif_thumbnail($file));
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/oom_exif_thumbnail_tiff.tif');
?>
--EXPECTF--
Warning: exif_thumbnail(): Thumbnail goes IFD boundary or end of file reached in %s on line %d
bool(false)
