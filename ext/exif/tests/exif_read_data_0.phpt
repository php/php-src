--TEST--
Test exif_read_data on unavailable file
--EXTENSIONS--
exif
--FILE--
<?php
$infile = __DIR__.'/bug73115.JPG';
exif_read_data($infile);
?>
--EXPECTF--
Warning: exif_read_data(): Unable to open file in %sexif_read_data_0.php on line %d
