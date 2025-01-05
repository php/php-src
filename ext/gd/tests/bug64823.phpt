--TEST--
Bug #64823 (ZTS GD fails to to find system TrueType font)
--EXTENSIONS--
gd
--FILE--
<?php
$font = "DejaVu Sans;Helvetica;Arial";
$im = imagecreatetruecolor(64, 64);
imagefilledrectangle($im, 0, 0, 63, 63, 0x000000);
var_dump(imagettftext($im, 16, 0, 10, 30, 0xffffff, $font, "hello") !== false);
?>
--EXPECT--
bool(true)
