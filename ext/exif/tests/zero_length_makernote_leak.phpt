--TEST--
OSS-Fuzz: Memory leak for zero-length MAKERNOTE
--SKIPIF--
<?php
if (!extension_loaded('exif')) {
    die('skip exif extension not available');
}
?>
--FILE--
<?php

@exif_read_data(__DIR__ . '/zero_length_makernote_leak.tiff');

?>
===DONE===
--EXPECT--
===DONE===
