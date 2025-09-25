--TEST--
HEIC box overflow
--EXTENSIONS--
exif
--FILE--
<?php
$bytearray = [
    0, 0, 0, 12, 'f', 't', 'y', 'p', 'h', 'e', 'i', 'c',
    0, 0, 0, 1, 'x', 'y', 'z', 'w', 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff-11, 0, 0, 0, 0, 0, 0, 0
];

function convert($x) {
    if (is_string($x)) return $x;
    return chr($x);
}

file_put_contents(__DIR__."/heic_box_overflow", implode('', array_map(convert(...), $bytearray)));

var_dump(exif_read_data(__DIR__."/heic_box_overflow"));
?>
--CLEAN--
<?php
@unlink(__DIR__."/heic_box_overflow");
?>
--EXPECTF--
Warning: exif_read_data(heic_box_overflow): Invalid HEIF file in %s on line %d
bool(false)
