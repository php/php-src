--TEST--
bfSize and biSizeImage fields of bitmap header match actual file size
--EXTENSIONS--
gd
--FILE--
<?php
const OFFSET_BF_SIZE = 2;
const OFFSET_BI_SIZE_IMAGE = 34;
const BIMAP_HEADER_SIZE = 14;
const INFO_HEADER_SIZE = 40;
const PALETTE_SIZE = 4;
const HEADER_SIZE = BIMAP_HEADER_SIZE + INFO_HEADER_SIZE + PALETTE_SIZE;

$im = imagecreate(19, 19);
imagecolorallocate($im, 0, 0, 0); // bg

ob_start();
imagebmp($im, null, false);
$data = ob_get_clean();
var_dump(substr($data, 0, 2));
var_dump(unpack("V", $data, OFFSET_BF_SIZE)[1] === strlen($data));
var_dump(unpack("V", $data, OFFSET_BI_SIZE_IMAGE)[1] === strlen($data) - HEADER_SIZE);

ob_start();
imagebmp($im, null, true);
$data = ob_get_clean();
var_dump(substr($data, 0, 2));
var_dump(unpack("V", $data, OFFSET_BF_SIZE)[1] === strlen($data));
var_dump(unpack("V", $data, OFFSET_BI_SIZE_IMAGE)[1] === strlen($data) - HEADER_SIZE);
?>
--EXPECT--
string(2) "BM"
bool(true)
bool(true)
string(2) "BM"
bool(true)
bool(true)
