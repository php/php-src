--TEST--
OSS-Fuzz: Temporary buffer leak in tag reading
--FILE--
<?php

var_dump(@exif_read_data(__DIR__ . '/temporary_buffer_leak.jpg'));

?>
--EXPECT--
bool(false)
