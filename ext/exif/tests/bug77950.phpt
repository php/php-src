--TEST--
Bug #77950 (Heap-buffer-overflow in _estrndup via exif_process_IFD_TAG)
--SKIPIF--
<?php if (!extension_loaded('exif')) print 'skip exif extension not available';?>
--FILE--
<?php
exif_read_data(__DIR__."/bug77950.tiff");
?>
DONE
--EXPECTF--
%A
DONE