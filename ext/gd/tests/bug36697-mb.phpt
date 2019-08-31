--TEST--
Bug #36697 (TrueColor transparency with GIF palette output).
--SKIPIF--
<?php
	if (!extension_loaded('gd')) {
		die("skip gd extension not available\n");
	}
?>
--FILE--
<?php
$dest = __DIR__ . "/36697私はガラスを食べられます.gif";

$im = imagecreatetruecolor(192, 36);
$trans_color = imagecolorallocate($im, 255, 0, 0);
$color = imagecolorallocate($im, 255, 255, 255);
imagecolortransparent($im, $trans_color);
imagefilledrectangle($im, 0,0, 192,36, $trans_color);
$c = imagecolorat($im, 191,35);
imagegif($im, $dest);
imagedestroy($im);
$im = imagecreatefromgif($dest);
$c = imagecolorat($im, 191, 35);
$colors = imagecolorsforindex($im, $c);
echo $colors['red'] . ' ' . $colors['green'] . ' ' . $colors['blue'];
@unlink($dest);
?>
--EXPECT--
255 0 0
