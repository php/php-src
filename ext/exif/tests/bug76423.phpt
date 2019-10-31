--TEST--
Bug #76423 (Int Overflow lead to Heap OverFlow in exif_thumbnail_extract of exif.c)
--SKIPIF--
<?php
if (!extension_loaded('exif')) die('skip exif extension not available');
?>
--FILE--
<?php
exif_read_data(__DIR__ . '/bug76423.jpg', 0, true, true);
?>
--EXPECTF--
Warning: exif_read_data(%s.jpg): Thumbnail goes IFD boundary or end of file reached in %s on line %d

Warning: exif_read_data(%s.jpg): File structure corrupted in %s on line %d

Warning: exif_read_data(%s.jpg): Invalid JPEG file in %s on line %d
