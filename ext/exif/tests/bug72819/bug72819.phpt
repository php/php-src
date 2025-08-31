--TEST--
Bug #72819 (EXIF thumbnails not read anymore)
--EXTENSIONS--
exif
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
