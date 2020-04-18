--TEST--
OSS-Fuzz: Temporary buffer leak in tag reading
--SKIPIF--
<?php
if (!extension_loaded('exif')) {
    die('skip exif extension not available');
}
?>
--FILE--
<?php

var_dump(@exif_read_data(__DIR__ . '/temporary_buffer_leak.jpg'));

?>
--EXPECT--
bool(false)
