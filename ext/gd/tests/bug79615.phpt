--TEST--
Bug #79615 (Wrong GIF header written in GD GIFEncode)
--EXTENSIONS--
gd
--FILE--
<?php
$im = imagecreate(3, 3); // 3x3, 9 colors, 4 bits per pixel
for ($x = 0; $x < 3; $x++) {
    for ($y = 0; $y < 3; $y++) {
        imagesetpixel($im, $x, $y, imagecolorallocate($im, $x, $y, 0));
    }
}
ob_start();
imagegif($im);
echo decbin(ord(ob_get_clean()[0xA]));
?>
--EXPECT--
10110011
