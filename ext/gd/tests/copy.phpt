--TEST--
imagecopy
--SKIPIF--
<?php
        if (!function_exists('imagecopy')) die("skip gd extension not available\n");
?>
--FILE--
<?php

$src_tc = imagecreatetruecolor(5,5);
imagefill($src_tc, 0,0, 0xffffff);
imagesetpixel($src_tc, 3,3, 0xff0000);
imagesetpixel($src_tc, 0,0, 0x0000ff);
imagesetpixel($src_tc, 4,4, 0x00ff00);


$dst_tc = imagecreatetruecolor(5,5);
imagecopy($dst_tc, $src_tc, 0,0, 0,0, imagesx($src_tc), imagesy($src_tc));
$p1 = imagecolorat($dst_tc, 3,3) == 0xff0000;
$p2 = imagecolorat($dst_tc, 0,0) == 0x0000ff;
$p3 = imagecolorat($dst_tc, 4,4) == 0x00ff00;

if ($p1 && $p2 && $p3) {
	echo "TC/TC: ok\n";
}

imagedestroy($src_tc); imagedestroy($dst_tc);


$src_tc = imagecreatetruecolor(5,5);
imagefill($src_tc, 0,0, 0xffffff);
imagesetpixel($src_tc, 3,3, 0xff0000);
imagesetpixel($src_tc, 0,0, 0x0000ff);
imagesetpixel($src_tc, 4,4, 0x00ff00);


$dst_tc = imagecreate(5,5);
imagecopy($dst_tc, $src_tc, 0,0, 0,0, imagesx($src_tc), imagesy($src_tc));

$c1 = imagecolorsforindex($dst_tc, imagecolorat($dst_tc, 3,3));
$c2 = imagecolorsforindex($dst_tc, imagecolorat($dst_tc, 0,0));
$c3 = imagecolorsforindex($dst_tc, imagecolorat($dst_tc, 4,4));

$p1 = $c1['red'] == 0xff && $c1['blue']==0x00 && $c1['green']==0x00;
$p2 = $c2['red'] == 0x00 && $c2['blue']==0xff && $c2['green']==0x00;
$p3 = $c3['red'] == 0x00 && $c3['blue']==0x00 && $c3['green']==0xff;

if ($p1 && $p2 && $p3) {
	echo "TC/P: ok\n";
}
imagedestroy($src_tc); imagedestroy($dst_tc);



$src_tc = imagecreate(5,5);
$c0 = imagecolorallocate($src_tc, 0xff, 0xff, 0xff);
$c1 = imagecolorallocate($src_tc, 0xff, 0x00, 0x00);
$c2 = imagecolorallocate($src_tc, 0x00, 0x00, 0xff);
$c3 = imagecolorallocate($src_tc, 0x00, 0xff, 0x00);

imagesetpixel($src_tc, 3,3, $c1);
imagesetpixel($src_tc, 0,0, $c2);
imagesetpixel($src_tc, 4,4, $c3);


$dst_tc = imagecreate(5,5);
imagecopy($dst_tc, $src_tc, 0,0, 0,0, imagesx($src_tc), imagesy($src_tc));

$c1 = imagecolorsforindex($dst_tc, imagecolorat($dst_tc, 3,3));
$c2 = imagecolorsforindex($dst_tc, imagecolorat($dst_tc, 0,0));
$c3 = imagecolorsforindex($dst_tc, imagecolorat($dst_tc, 4,4));

$p1 = $c1['red'] == 0xff && $c1['blue']==0x00 && $c1['green']==0x00;
$p2 = $c2['red'] == 0x00 && $c2['blue']==0xff && $c2['green']==0x00;
$p3 = $c3['red'] == 0x00 && $c3['blue']==0x00 && $c3['green']==0xff;


if ($p1 && $p2 && $p3) {
	echo "P/P: ok\n";
}



$src_tc = imagecreate(5,5);
$c0 = imagecolorallocate($src_tc, 0xff, 0xff, 0xff);
$c1 = imagecolorallocate($src_tc, 0xff, 0x00, 0x00);
$c2 = imagecolorallocate($src_tc, 0x00, 0x00, 0xff);
$c3 = imagecolorallocate($src_tc, 0x00, 0xff, 0x00);

imagesetpixel($src_tc, 3,3, $c1);
imagesetpixel($src_tc, 0,0, $c2);
imagesetpixel($src_tc, 4,4, $c3);


$dst_tc = imagecreatetruecolor(5,5);
imagecopy($dst_tc, $src_tc, 0,0, 0,0, imagesx($src_tc), imagesy($src_tc));
$p1 = imagecolorat($dst_tc, 3,3) == 0xff0000;
$p2 = imagecolorat($dst_tc, 0,0) == 0x0000ff;
$p3 = imagecolorat($dst_tc, 4,4) == 0x00ff00;

if ($p1 && $p2 && $p3) {
	echo "P/TC: ok\n";
}
?>
--EXPECT--
TC/TC: ok
TC/P: ok
P/P: ok
P/TC: ok
