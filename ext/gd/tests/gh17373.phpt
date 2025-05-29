--TEST--
Bug GH-17373 (imagefttext() ignores clipping rect for palette images)
--EXTENSIONS--
gd
--SKIPIF--
<?php
if(!function_exists('imagefttext')) die('skip imagefttext() not available');
?>
--FILE--
<?php
$im = imagecreate(64, 32);
$bg = imagecolorallocate($im, 0, 0, 0);
$fg = imagecolorallocate($im, 255, 255, 255);
imagefilledrectangle($im, 0, 0, 63, 31, $bg);
imagesetclip($im, 32, 0, 63, 31);
imagefttext($im, 16, 0, 10, 23, $fg, __DIR__ . "/Tuffy.ttf", "hello");

imagesetclip($im, 0, 0, 63, 31);
$count = 0;
for ($j = 0; $j < 31; $j++) {
    for ($i = 0; $i < 31; $i++) {
        if (imagecolorat($im, $i, $j) !== $bg) {
            $count++;
        }
    }
}
var_dump($count);
?>
--EXPECT--
int(0)
