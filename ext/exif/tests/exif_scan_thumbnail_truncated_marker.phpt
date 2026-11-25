--TEST--
exif_thumbnail(): truncated JPEG marker segment at the end of the thumbnail
--EXTENSIONS--
exif
--FILE--
<?php
// Little-endian TIFF: empty IFD0, IFD1 pointing at a 5 byte thumbnail
// (FF D8 FF E0 00) at the very end of the data. The marker length field
// of the APP0 segment would start at the last byte, so reading its two
// bytes must not run past the thumbnail buffer.
$thumb = "\xFF\xD8\xFF\xE0\x00";
$tiff = "II*\x00" . pack("V", 8)
    . pack("v", 0) . pack("V", 14)
    . pack("v", 2)
    . pack("vvVV", 0x0201, 4, 1, 44)
    . pack("vvVV", 0x0202, 4, 1, strlen($thumb))
    . pack("V", 0)
    . $thumb;

$stream = fopen("php://memory", "w+");
fwrite($stream, $tiff);
rewind($stream);

var_dump(strlen(exif_thumbnail($stream, $width, $height, $type)));
var_dump($width, $height);
?>
--EXPECT--
int(5)
int(0)
int(0)
