--TEST--
PHP crash when zend_multibyte_encoding_converter returns (size_t)-1)
--SKIPIF--
<?php if (!extension_loaded('exif')) print 'skip exif extension not available';?>
--FILE--
<?php
$infile = dirname(__FILE__).'/exif_encoding_crash.jpg';
var_dump(exif_read_data($infile));
?>
===DONE===
--EXPECTF--
Warning: exif_read_data(exif_encoding_crash.jpg): Process tag(x9003=DateTimeOri): Illegal pointer offset(%s) in %s on line %d

Warning: exif_read_data(exif_encoding_crash.jpg): Error reading from file: got=x%x(=%d) != itemlen-%d=x%x(=%d) in %s on line %d

Warning: exif_read_data(exif_encoding_crash.jpg): Invalid JPEG file in %s on line %d
bool(false)
===DONE===

