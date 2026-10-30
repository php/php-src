--TEST--
GIF OOB array access when using code size of 12
--EXTENSIONS--
gd
--FILE--
<?php

$fileHeaderParts = [
	"signature" => "GIF",
	"version" => "89a",
];
$fileHeader = implode("", $fileHeaderParts);

$logicalScreenDescriptorParts = [
	// little-endian format
	"width" => "\x04\x00",
	"height" => "\x04\x00",
	// packed data: global color table flag (most significant bit),
	// color resolution (3 bits, only meaningful if global color table is enabled
	// and we don't enable it here)
	// sort flag (one bit, again only meaningful if global color table is enabled)
	// size of global color table (3 bits)
	"packed_info" => "\x00",
	"background_color_index" => "\x00",
	"pixel_aspect_ratio" => "\x00",
];
$logicalScreenDescriptor = implode("", $logicalScreenDescriptorParts);

$startImage = ",";

$imgDescParts = [
	// little-ending format
	"left" => "\x00\x00",
	"top" => "\x00\x00",
	"width" => "\x04\x00",
	"height" => "\x04\x00",
	// more packed data: local color table flag, interlace flag, sort flag,
	// 2 bits reserved for future use, then 3 bits for size of local color
	// table, which we don't have
	"packed_info" => "\x00",
];
$imgDesc = implode("", $imgDescParts);

$imgDataParts = [
	"lzw_min_code_size" => "\x0c", // 12
	"sub_block_num_bytes" => "\x05",
	// Data in the block: 3 12-bit codes, and then 4 trailing 0 bits
	"sub_block_bytes" => "\xff\x5f\x00\x06\x40",
	// end of data
	"end" => "\x00"
];
$imgData = implode("", $imgDataParts);

$trailer = ";";

$source = $fileHeader . $logicalScreenDescriptor . $startImage . $imgDesc . $imgData . $trailer;
$img = imagecreatefromstring($source);
var_dump($img);

?>
--EXPECTF--
object(GdImage)#%d (0) {
}
