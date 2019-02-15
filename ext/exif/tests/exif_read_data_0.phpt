--TEST--
Test exif_read_data on unavailable file
--SKIPIF--
<?php if (!extension_loaded('exif')) print 'skip exif extension not available';?>
--FILE--
<?php
$infile = dirname(__FILE__).'/bug73115.JPG';
exif_read_data($infile);
?>
===DONE===
--EXPECTF--
Warning: exif_read_data(): Unable to open file in %sexif_read_data_0.php on line %d
===DONE===
