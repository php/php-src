--TEST--
Bug #73115 (exif_read_data triggers warning on reading binary strings)
--SKIPIF--
<?php if (!extension_loaded('exif')) print 'skip exif extension not available';?>
--INI--
output_handler=
zlib.output_compression=0
--FILE--
<?php
$infile = __DIR__.'/bug73115.jpg';
var_dump(count(exif_read_data($infile)));
?>
--EXPECT--
int(80)
