--TEST--
bfSize field of bitmap header matches actual file size
--EXTENSIONS--
gd
--FILE--
<?php
$im = imagecreate(19, 19);
imagecolorallocate($im, 0, 0, 0); // bg
ob_start();
imagebmp($im, null, false);
$data = ob_get_clean();
var_dump(substr($data, 0, 2));
var_dump(unpack("V", $data, 2)[1] === strlen($data));
?>
--EXPECT--
string(2) "BM"
bool(true)
