--TEST--
Bug #42434 (ImageLine w/ antialias = 1px shorter)
--EXTENSIONS--
gd
--FILE--
<?php
$im = imagecreatetruecolor(10, 2);
imagefilledrectangle($im, 0, 0, 10, 2, 0xFFFFFF);

imageantialias($im, true);
imageline($im, 0, 0, 10, 0, 0x000000);

if (imagecolorat($im, 9, 0) == 0x000000) {
    echo 'DONE';
} else {
    echo 'Bugged';
}

imagedestroy($im);
?>
--EXPECT--
DONE
