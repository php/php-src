--TEST--
Bug #77988 (heap-buffer-overflow on php_jpg_get16)
--SKIPIF--
<?php if (!extension_loaded('exif')) print 'skip exif extension not available';?>
--FILE--
<?php
exif_read_data(__DIR__."/bug77988.jpg", 'COMMENT', FALSE, TRUE);
?>
DONE
--EXPECTF--
DONE