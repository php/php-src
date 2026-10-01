--TEST--
GH-11020 (exif_read_data() emits a spurious "Illegal IFD size" warning when an IFD is not followed by a next-IFD offset)
--EXTENSIONS--
exif
--FILE--
<?php
$data = exif_read_data(__DIR__ . '/gh11020.jpg');
var_dump(is_array($data), $data['Orientation']);
?>
--EXPECT--
bool(true)
int(1)
