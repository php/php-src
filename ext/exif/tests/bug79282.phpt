--TEST--
Bug #79282: Use-of-uninitialized-value in exif
--EXTENSIONS--
exif
--FILE--
<?php

var_dump(exif_read_data('data://image/jpeg;base64,/9jhAAlFeGlmAAAg'));

?>
--EXPECTF--
Warning: exif_read_data(): Missing TIFF alignment marker in %s on line %d

Warning: exif_read_data(): File structure corrupted in %s on line %d

Warning: exif_read_data(): Invalid JPEG file in %s on line %d
bool(false)
