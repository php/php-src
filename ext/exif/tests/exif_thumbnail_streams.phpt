--TEST--
exif_thumbnail() with streams test
--SKIPIF--
<?php if (!extension_loaded('exif')) print 'skip exif extension not available';?>
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
