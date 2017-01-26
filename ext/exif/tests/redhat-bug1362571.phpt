--TEST--
Redhat bug #1362571 (PHP not returning full results for exif_read_data function)
--SKIPIF--
<?php if (!extension_loaded('exif')) print 'skip exif extension not available';?>
--FILE--
<?php
var_dump(strlen(exif_thumbnail(__DIR__ . DIRECTORY_SEPARATOR . 'redhat-bug1362571.jpg')) > 0);
?>
--EXPECT--
bool(true)
