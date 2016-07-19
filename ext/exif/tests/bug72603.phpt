--TEST--
Bug #72603 (Out of bound read in exif_process_IFD_in_MAKERNOTE)
--SKIPIF--
<?php if (!extension_loaded('exif')) print 'skip exif extension not available';?>
--FILE--
<?php
var_dump(count(exif_read_data(dirname(__FILE__) . "/bug72603.jpeg")));
?>
--EXPECTF--
Warning: exif_read_data(bug72603.jpeg): IFD data bad offset: 0x058C length 0x001C in %s/bug72603.php on line %d
int(13)