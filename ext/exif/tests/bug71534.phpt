--TEST--
Bug #71534 (Type confusion in exif_read_data() leading to heap overflow in debug mode)
--EXTENSIONS--
exif
--FILE--
<?php
// This is kinda bad, I know! But, this generates about 200+ warnings due to its
// broken TIFF format
var_dump(@exif_read_data(__DIR__ . DIRECTORY_SEPARATOR . 'bug71534.tiff') === false);
?>
--EXPECT--
bool(true)
