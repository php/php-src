--TEST--
Bug #77753 (Heap-buffer-overflow in php_ifd_get32s)
--SKIPIF--
<?php if (!extension_loaded('exif')) print 'skip exif extension not available';?>
--FILE--
<?php
var_dump(exif_read_data(__DIR__."/bug77753.tiff"));
?>
DONE
--EXPECTF--
%A
Warning: exif_read_data(bug77753.tiff): Illegal IFD size: 0x006A > 0x0065 in %sbug77753.php on line %d

Warning: exif_read_data(bug77753.tiff): Invalid TIFF file in %sbug77753.php on line %d
bool(false)
DONE