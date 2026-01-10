--TEST--
OSS-Fuzz #442954659 (zero-size box in HEIF file causes infinite loop)
--EXTENSIONS--
exif
--FILE--
<?php
exif_read_data(__DIR__."/input");
?>
--EXPECTF--
Warning: exif_read_data(%s): Invalid HEIF file in %s on line %d
