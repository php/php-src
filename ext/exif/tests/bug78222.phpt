--TEST--
Bug #78222 (heap-buffer-overflow on exif_scan_thumbnail)
--SKIPIF--
<?php if (!extension_loaded('exif')) print 'skip exif extension not available';?>
--FILE--
<?php
exif_read_data(__DIR__."/bug78222.jpg", 'THUMBNAIL', FALSE, TRUE);
?>
DONE
--EXPECT--
DONE
