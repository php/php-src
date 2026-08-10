--TEST--
HEIC meta box size must be bounded by the file size
--EXTENSIONS--
exif
--INI--
memory_limit=32M
--FILE--
<?php
// ftyp box (size 20) followed by a meta box whose size field claims 128MB,
// in a file that is only 37 bytes. Without an upper bound the meta box
// allocation exhausts memory_limit before any read is attempted.
$ftyp = pack("N", 20) . "ftypheic" . str_repeat("\x00", 8);
$meta = pack("N", 0x08000000) . "meta" . str_repeat("\x00", 8);
file_put_contents(__DIR__."/heic_meta_box_alloc.heic", $ftyp . $meta . "\x00");
var_dump(exif_read_data(__DIR__."/heic_meta_box_alloc.heic"));
?>
--CLEAN--
<?php
@unlink(__DIR__."/heic_meta_box_alloc.heic");
?>
--EXPECTF--
Warning: exif_read_data(heic_meta_box_alloc.heic): Invalid HEIF file in %s on line %d
bool(false)
