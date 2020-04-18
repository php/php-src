--TEST--
Bug #72819 (EXIF thumbnails not read anymore)
--SKIPIF--
<?php if (!extension_loaded('exif')) print 'skip exif extension not available';?>
--INI--
output_handler=
zlib.output_compression=0
--FILE--
<?php
$infile = __DIR__.'/bug72819.jpg';
var_dump(strlen(exif_thumbnail($infile)));
?>
--EXPECT--
int(5448)
