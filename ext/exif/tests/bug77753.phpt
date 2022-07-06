--TEST--
Bug #77753 (Heap-buffer-overflow in php_ifd_get32s)
--SKIPIF--
<?php if (!extension_loaded('exif')) print 'skip exif extension not available';?>
--FILE--
<?php
@var_dump(exif_read_data(__DIR__."/bug77753.tiff"));
?>
DONE
--EXPECT--
bool(false)
DONE
