--TEST--
Bug #38179 (imagecopy from a palette to a truecolor image loses alpha channel)
--SKIPIF--
<?php
        if (!function_exists('imagecopy')) die("skip gd extension not available\n");
?>
--FILE--
<?php
$src = imagecreate(5,5);
$c0 = imagecolorallocate($src, 255,255,255);
$c1 = imagecolorallocatealpha($src, 255,0,0,70);

imagealphablending($src, 0);
imagefill($src, 0,0, $c1);

$dst_tc = imagecreatetruecolor(5,5);
imagealphablending($dst_tc, 0);

imagecopy($dst_tc, $src, 0,0, 0,0, imagesx($src), imagesy($src));

$p1 = imagecolorat($dst_tc, 3,3);
printf("%X\n", $p1);

imagedestroy($src); imagedestroy($dst_tc);
?>
--EXPECTF--
46FF0000
