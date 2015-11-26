--TEST--
imagecolorat
--SKIPIF--
<?php
        if (!function_exists('imagecolorat')) die("skip gd extension not available\n");
?>
--FILE--
<?php
$file = dirname(__FILE__) . '/im.wbmp';

$im = imagecreatetruecolor(6,6);
imagefill($im, 0,0, 0xffffff);
imagesetpixel($im, 3,3, 0x0);


echo 'test colorat truecolor: ';
$c = imagecolorat($im, 3,3);
echo $c == 0x0 ? 'ok' : 'failed';
echo "\n";
imagedestroy($im);

$im = imagecreate(6,6);
$c1 = imagecolorallocate($im, 255,255,255);
$c2 = imagecolorallocate($im, 0,0,0);

imagefill($im, 0,0, $c1);
imagesetpixel($im, 3,3, $c2);
echo 'test colorat palette: ';

$c = imagecolorsforindex($im, imagecolorat($im, 3,3));
$failed = false;
foreach ($c as $v) {
	if ($v != 0) {
		$failed = true;
	}
}
echo !$failed ? 'ok' : 'failed';
echo "\n";
?>
--EXPECT--
test colorat truecolor: ok
test colorat palette: ok
