--TEST--
Bug #77831 (Heap-buffer-overflow in exif_iif_add_value in EXIF)
--SKIPIF--
<?php if (!extension_loaded('exif')) print 'skip exif extension not available';?>
--FILE--
<?php
var_dump(exif_read_data(__DIR__."/bug77831.tiff"));
?>
DONE
--EXPECTF--
%A
bool(false)
DONE