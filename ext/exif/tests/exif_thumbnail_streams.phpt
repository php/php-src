--TEST--
exif_thumbnail() with streams test
--EXTENSIONS--
exif
--INI--
output_handler=
zlib.output_compression=0
--FILE--
<?php
$fp = fopen(__DIR__ . '/sony.jpg', 'rb');

var_dump(strlen(exif_thumbnail($fp)));

exif_thumbnail($fp, width: $width);
var_dump($width);

exif_thumbnail($fp, height: $height);
var_dump($height);

exif_thumbnail($fp, image_type: $image_type);
var_dump($image_type == IMAGETYPE_JPEG);

fclose($fp);
?>
--EXPECT--
int(4150)
int(160)
int(90)
bool(true)
