--TEST--
Redhat bug #1362571 (PHP not returning full results for exif_read_data function)
--EXTENSIONS--
exif
--FILE--
<?php
var_dump(strlen(exif_thumbnail(__DIR__ . DIRECTORY_SEPARATOR . 'redhat-bug1362571.jpg')) > 0);
?>
--EXPECT--
bool(true)
