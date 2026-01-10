--TEST--
HEIC iloc extent_length underflow
--EXTENSIONS--
exif
--FILE--
<?php
// Read valid HEIC file and patch iloc extent_length to 1
$data = file_get_contents(__DIR__."/image029.heic");
$data = substr_replace($data, "\x00\x00\x00\x01", 0x4f8, 4);
file_put_contents(__DIR__."/heic_iloc_underflow.heic", $data);
var_dump(exif_read_data(__DIR__."/heic_iloc_underflow.heic"));
?>
--CLEAN--
<?php
@unlink(__DIR__."/heic_iloc_underflow.heic");
?>
--EXPECTF--
Warning: exif_read_data(heic_iloc_underflow.heic): Invalid HEIF file in %s on line %d
bool(false)
