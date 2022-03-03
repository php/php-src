--TEST--
Bug #73115 (exif_read_data triggers warning on reading binary strings)
--EXTENSIONS--
exif
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
