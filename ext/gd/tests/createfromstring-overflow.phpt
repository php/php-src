--TEST--
imagecreatefromstring overflow with compressed chunk of size INT_MAX
--EXTENSIONS--
gd
--FILE--
<?php

// Documentation available at https://libgd.github.io/manuals/2.3.3/files/gd_gd2-c.html
$fileHeaderParts = [
	"signature" => "gd2\x00",
	"version" => "\x00\x02",
	"width" => "\x00\x01",
	"height" => "\x00\x01",
	"chunk_size" => "\x00\x40",
	"format" => "\x00\x04", // compressed truecolor image data
	"x_chunk_count" => "\x00\x01",
	"y_chunk_count" => "\x00\x01",
];
$fileHeader = implode("", $fileHeaderParts);

$chunkHeaderParts = [
	"offset" => "\x00\x00\x00\x20",
	"size" =>"\x7F\xFF\xFF\xFF", // INT_MAX
];
$chunkHeader = implode("", $chunkHeaderParts);

$trueColorHeaderParts = [
	"truecolor" => "\x01",
	"transparent" => "\x00\x00\x00\x00",
];
$trueColorHeader = implode("", $trueColorHeaderParts);

$source = $fileHeader . $chunkHeader . $trueColorHeader;
$img = imagecreatefromstring($source);
var_dump($img);

?>
--EXPECTF--
Warning: imagecreatefromstring(): Passed data is not in "GD2" format in %s on line %d

Warning: imagecreatefromstring(): Couldn't create GD Image Stream out of Data in %s on line %d
bool(false)
