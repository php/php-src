--TEST--
Bug #74428 (Unsigned rational numbers are written out as signed rationals)
--EXTENSIONS--
exif
--INI--
output_handler=
zlib.output_compression=0
--FILE--
<?php
$infile = __DIR__.'/DJI_0245_tiny.jpg';
$tags = exif_read_data($infile);
echo $tags['GPSLatitude'][2], PHP_EOL;
echo $tags['GPSLongitude'][2], PHP_EOL;
?>
--EXPECT--
156691934/70000000
2672193571/70000000
