--TEST--
Bug #60150 (Integer overflow during the parsing of invalid exif header)
--SKIPIF--
<?php if (!extension_loaded('exif')) print 'skip exif extension not available';?>
--INI--
output_handler=
zlib.output_compression=0 
--FILE--
<?php
$infile = dirname(__FILE__).'/bug60150.jpg';
var_dump(exif_read_data($infile));
?>
===DONE===
--EXPECTF--
Warning: exif_read_data(bug60150.jpg): Process tag(x9003=DateTimeOri): Illegal pointer offset(%s) in %s on line %d

Warning: exif_read_data(bug60150.jpg): Error reading from file: got=x%x(=%d) != itemlen-%d=x%x(=%d) in %s on line %d

Warning: exif_read_data(bug60150.jpg): Invalid JPEG file in %s on line %d
bool(false)
===DONE===
