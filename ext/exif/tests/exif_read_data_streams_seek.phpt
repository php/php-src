--TEST--
exif_read_data() with streams seeking test
--SKIPIF--
<?php if (!extension_loaded('exif')) print 'skip exif extension not available';?>
--INI--
output_handler=
zlib.output_compression=0
--FILE--
<?php
$fp = fopen(__DIR__ . '/image027.tiff', 'rb');

fseek($fp, 100, SEEK_SET);

exif_read_data($fp);

var_dump(ftell($fp) === 100);

fclose($fp);
?>
--EXPECT--
bool(true)
