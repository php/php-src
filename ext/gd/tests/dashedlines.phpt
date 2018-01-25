--TEST--
imageline, dashed
--SKIPIF--
<?php
        if (!function_exists('imagecreatefromstring')) die("skip gd extension not available\n");
?>
--FILE--
<?php
$im = imagecreatetruecolor(6,6);
imagefill($im, 0,0, 0xffffff);

$r  = 0xff0000;
$b = 0x0000ff;

$style = array($r, $b);
imagesetstyle($im, $style);

// Horizontal line
imageline($im, 0,5, 5,5, IMG_COLOR_STYLED);
$p1 = imagecolorat($im, 0,5) == $r;
$p2 = imagecolorat($im, 1,5) == $b;
$p3 = imagecolorat($im, 2,5) == $r;
$p4 = imagecolorat($im, 3,5) == $b;
$p5 = imagecolorat($im, 4,5) == $r;
$p5 = imagecolorat($im, 5,5) == $b;


if ($p1 && $p2 && $p3 && $p4 && $p5) {
	echo "Horizontal: ok\n";
}
imagedestroy($im);

$im = imagecreatetruecolor(6,6);
imagefill($im, 0,0, 0xffffff);

$style = array($r, $b);
imagesetstyle($im, $style);


imageline($im, 2,0, 2,5, IMG_COLOR_STYLED);
$p1 = imagecolorat($im, 2,0) == $r;
$p2 = imagecolorat($im, 2,1) == $b;
$p3 = imagecolorat($im, 2,2) == $r;
$p4 = imagecolorat($im, 2,3) == $b;
$p5 = imagecolorat($im, 2,4) == $r;
$p6 = imagecolorat($im, 2,5) == $b;

if ($p1 && $p2 && $p3 && $p4 && $p5 && $p6) {
	echo "Vertical: ok\n";
}
imagedestroy($im);


$im = imagecreatetruecolor(6,6);
imagefill($im, 0,0, 0xffffff);

$style = array($r, $b);
imagesetstyle($im, $style);

imageline($im, 0,0, 5,5, IMG_COLOR_STYLED);
$p1 = imagecolorat($im, 0,0) == $r;
$p2 = imagecolorat($im, 1,1) == $b;
$p3 = imagecolorat($im, 2,2) == $r;
$p4 = imagecolorat($im, 3,3) == $b;
$p5 = imagecolorat($im, 4,4) == $r;
$p6 = imagecolorat($im, 5,5) == $b;

if ($p1 && $p2 && $p3 && $p4 && $p5 && $p6) {
	echo "Diagonal: ok\n";
}
imagedestroy($im);


?>
--EXPECTF--
Horizontal: ok
Vertical: ok
Diagonal: ok
