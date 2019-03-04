--TEST--
Bug 77563 (Uninitialized read in exif_process_IFD_in_MAKERNOTE)
--SKIPIF--
<?php if (!extension_loaded('exif')) print 'skip exif extension not available';?>
--FILE--
<?php
$s = exif_thumbnail(__DIR__."/bug77563.jpg");
?>
DONE
--EXPECTF--
Warning: exif_thumbnail(bug77563.jpg): IFD data too short: 0x0009 offset 0x0008 in %s%ebug77563.php on line %d

Warning: exif_thumbnail(bug77563.jpg): File structure corrupted in %s%ebug77563.php on line %d

Warning: exif_thumbnail(bug77563.jpg): Invalid JPEG file in %s%ebug77563.php on line %d
DONE
