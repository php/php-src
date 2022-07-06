--TEST--
imagecopyresized
--SKIPIF--
<?php
        if (!function_exists('imagecopyresized')) die("skip gd extension not available\n");
?>
--FILE--
<?php

function get_hexcolor($im, $c) {
    if (imageistruecolor($im)) {
        return $c;
    }
    $colors = imagecolorsforindex($im, $c);
    return ($colors['red'] << 16)  + ($colors['green'] << 8) + ($colors['blue']);
}

function check_doublesize($dst) {
    $im = imagecreatetruecolor(38,38);
    imagefill($im,0,0, 0xffffff);
    imagefilledrectangle($im, 0,0,9,9, 0xff0000);
    imagefilledrectangle($im, 0,28,9,37, 0xff0000);
    imagefilledrectangle($im, 28,0,37,9, 0xff0000);
    imagefilledrectangle($im, 28,28,37,37, 0xff0000);
    imagefilledrectangle($im, 14,14,23,23, 0xff0000);

    for ($x = 0; $x < 38; $x++) {
        for ($y = 0; $y < 38; $y++) {
            $p1 = imagecolorat($im, $x, $y);
            $p2 = imagecolorat($dst, $x, $y);
            if (get_hexcolor($im, $p1) != get_hexcolor($dst, $p2)) {
                return false;
            }
        }
    }
    return true;
}

$src_tc = imagecreatetruecolor(19,19);
imagefill($src_tc, 0,0, 0xffffff);
imagefilledrectangle($src_tc, 0,0,4,4, 0xff0000);
imagefilledrectangle($src_tc, 14,0,18,4, 0xff0000);
imagefilledrectangle($src_tc, 0,14,4,18, 0xff0000);
imagefilledrectangle($src_tc, 14,14,18,18, 0xff0000);
imagefilledrectangle($src_tc, 7,7,11,11, 0xff0000);

$dst_tc = imagecreatetruecolor(38,38);
imagecopyresized($dst_tc, $src_tc, 0,0, 0,0, imagesx($dst_tc), imagesy($dst_tc), 19,19);

if (!check_doublesize($dst_tc)) exit("1 failed\n");
echo "TC->TC: ok\n";

$src_tc = imagecreate(19,19);
$white = imagecolorallocate($src_tc, 255,255,255);
$red = imagecolorallocate($src_tc, 255,0,0);

imagefilledrectangle($src_tc, 0,0,4,4, $red);
imagefilledrectangle($src_tc, 14,0,18,4, $red);
imagefilledrectangle($src_tc, 0,14,4,18, $red);
imagefilledrectangle($src_tc, 14,14,18,18, $red);
imagefilledrectangle($src_tc, 7,7,11,11, $red);

$dst_tc = imagecreatetruecolor(38,38);
imagecopyresized($dst_tc, $src_tc, 0,0, 0,0, imagesx($dst_tc), imagesy($dst_tc), 19,19);

if (!check_doublesize($dst_tc)) exit("2 failed\n");
echo "P->TC: ok\n";

$src_tc = imagecreate(19,19);
$white = imagecolorallocate($src_tc, 255,255,255);
$red = imagecolorallocate($src_tc, 255,0,0);

imagefilledrectangle($src_tc, 0,0,4,4, $red);
imagefilledrectangle($src_tc, 14,0,18,4, $red);
imagefilledrectangle($src_tc, 0,14,4,18, $red);
imagefilledrectangle($src_tc, 14,14,18,18, $red);
imagefilledrectangle($src_tc, 7,7,11,11, $red);

$dst_tc = imagecreate(38,38);
$white = imagecolorallocate($src_tc, 255,255,255);
$red = imagecolorallocate($src_tc, 255,0,0);

imagecopyresized($dst_tc, $src_tc, 0,0, 0,0, imagesx($dst_tc), imagesy($dst_tc), 19,19);

if (!check_doublesize($dst_tc)) exit("3 failed\n");
echo "P->P: ok\n";
?>
--EXPECT--
TC->TC: ok
P->TC: ok
P->P: ok
