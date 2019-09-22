--TEST--
OSS-Fuzz: Memory leak for zero-length MAKERNOTE
--FILE--
<?php

@exif_read_data(__DIR__ . '/zero_length_makernote_leak.tiff');

?>
===DONE===
--EXPECT--
===DONE===
