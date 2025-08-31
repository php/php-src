--TEST--
Bug #60150 (Integer overflow during the parsing of invalid exif header)
--EXTENSIONS--
exif
--INI--
output_handler=
zlib.output_compression=0
--FILE--
<?php
$infile = __DIR__.'/bug60150.jpg';
var_dump(exif_read_data($infile));
?>
--EXPECTF--
Warning: exif_read_data(bug60150.jpg): Process tag(x9003=DateTimeOriginal): Illegal pointer offset(%s) in %s on line %d

Warning: exif_read_data(bug60150.jpg): Error reading from file: got=x%x(=%d) != itemlen-%d=x%x(=%d) in %s on line %d

Warning: exif_read_data(bug60150.jpg): Invalid JPEG file in %s on line %d
bool(false)
