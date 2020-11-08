--TEST--
OSS-Fuzz #17163: Out-of-bounds read due to tag with zero components
--SKIPIF--
<?php
if (!extension_loaded('exif')) {
    die('skip exif extension not available');
}
?>
--FILE--
<?php

var_dump(exif_read_data(__DIR__ . '/tag_with_illegal_zero_components.jpeg'));

?>
--EXPECTF--
Warning: exif_read_data(tag_with_illegal_zero_components.jpeg): Process tag(x0202=JPEGInterchangeFormatLength): Cannot be empty in %s on line %d

Warning: exif_read_data(tag_with_illegal_zero_components.jpeg): File structure corrupted in %s on line %d

Warning: exif_read_data(tag_with_illegal_zero_components.jpeg): Invalid JPEG file in %s on line %d
bool(false)
