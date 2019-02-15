--TEST--
Bug #74428 (Unsigned rational numbers are written out as signed rationals)
--SKIPIF--
<?php if (!extension_loaded('exif')) print 'skip exif extension not available';?>
--INI--
output_handler=
zlib.output_compression=0
--FILE--
<?php
$infile = dirname(__FILE__).'/DJI_0245_tiny.JPG';
$tags = exif_read_data($infile);
echo $tags['GPSLatitude'][2], PHP_EOL;
echo $tags['GPSLongitude'][2], PHP_EOL;
?>
===DONE===
--EXPECTF--
156691934/70000000
2672193571/70000000
===DONE===
