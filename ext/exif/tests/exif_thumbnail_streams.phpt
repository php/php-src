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

fclose($fp);
?>
--EXPECT--
int(4150)
