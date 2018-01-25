--TEST--
gif in/out
--SKIPIF--
<?php
// $Id$
	if (!extension_loaded('gd')) {
		die("skip gd extension not available.");
	}
	if (!function_exists("imagegif") || !function_exists("imagecreatefromgif") || !function_exists('imagecreatefromjpeg')) {
		die("skip gif support unavailable");
	}
?>
--FILE--
<?php

function check_box($r,$g,$b, $error=0) {
	$cwd = dirname(__FILE__);
	$im2 = imagecreatefromgif($cwd . '/test_gif.gif');

	$c = imagecolorsforindex($im2, imagecolorat($im2, 8,8));

	if ($error>0) {
		$r_min = $r - $error; $r_max = $r + $error;
		$g_min = $g - $error; $g_max = $g + $error;
		$b_min = $b - $error; $b_max = $b + $error;

		if (
			($c['red'] >= $r_min || $c['red'] <= $r_max)
			&&
			($c['green'] >= $g_min || $c['green'] <= $g_max)
			&&
			($c['blue'] >= $b_min || $c['blue'] <= $b_max)
			) {
			return true;
		} else {
			return false;
		}
	} else {
		if ($c['red']==$r && $c['green']==$g && $c['blue']==$b) {
			return true;
		} else {
			return false;
		}
	}
}
$cwd = dirname(__FILE__);

$im = imagecreate(10,10);
$c = imagecolorallocate($im, 255,255,255);
imagefilledrectangle($im, 5,5, 10,10, $c);
imagegif($im, $cwd . '/test_gif.gif');
if (check_box(255,255,255)) {
	echo "<4 cols: ok\n";
}

$im = imagecreate(10,10);
for ($i=0; $i<7; $i++) {
	$c = imagecolorallocate($im, $i,$i,$i);
}
imagefilledrectangle($im, 5,5, 10,10, $c);
imagegif($im, $cwd . '/test_gif.gif');
$i--;
if (check_box($i,$i,$i)) {
		echo "<8 cols: ok\n";
}


$im = imagecreate(10,10);
for ($i=0; $i<15; $i++) {
	$c = imagecolorallocate($im, $i,$i,$i);
}
imagefilledrectangle($im, 5,5, 10,10, $c);
imagegif($im, $cwd . '/test_gif.gif');
$i--;
if (check_box($i,$i,$i)) {
		echo "<16 cols: ok\n";
}


$im = imagecreate(10,10);
for ($i=0; $i<31; $i++) {
	$c = imagecolorallocate($im, $i,$i,$i);
}
imagefilledrectangle($im, 5,5, 10,10, $c);
imagegif($im, $cwd . '/test_gif.gif');
$i--;
if (check_box($i,$i,$i)) {
		echo "<32 cols: ok\n";
}


$im = imagecreate(10,10);
for ($i=0; $i<63; $i++) {
	$c = imagecolorallocate($im, $i,$i,$i);
}
imagefilledrectangle($im, 5,5, 10,10, $c);
imagegif($im, $cwd . '/test_gif.gif');
$i--;
if (check_box($i,$i,$i)) {
		echo "<64 cols: ok\n";
}

$im = imagecreate(10,10);
for ($i=0; $i<127; $i++) {
	$c = imagecolorallocate($im, $i,$i,$i);
}
imagefilledrectangle($im, 5,5, 10,10, $c);
imagegif($im, $cwd . '/test_gif.gif');
$i--;
if (check_box($i,$i,$i)) {
		echo "<128 cols: ok\n";
}

$im = imagecreate(10,10);
for ($i=0; $i<255; $i++) {
	$c = imagecolorallocate($im, $i,$i,$i);
}
imagefilledrectangle($im, 5,5, 10,10, $c);
imagegif($im, $cwd . '/test_gif.gif');
$i--;
if (check_box($i,$i,$i)) {
		echo "<256 cols: ok\n";
}


$im = imagecreatefromjpeg($cwd . '/conv_test.jpeg');
imagefilledrectangle($im, 5,5, 10,10, 0xffffff);
imagegif($im, $cwd . '/test_gif.gif');
imagegif($im, $cwd . '/test_gif_2.gif');

if (check_box(255,255,255, 10)) {
	echo ">256 cols: ok\n";
}

@unlink($cwd . "/test_gif.gif");
@unlink($cwd . "/test_gif_2.gif");
?>
--EXPECT--
<4 cols: ok
<8 cols: ok
<16 cols: ok
<32 cols: ok
<64 cols: ok
<128 cols: ok
<256 cols: ok
>256 cols: ok
