--TEST--
imagegd() writes truecolor images without palette conversion
--SKIPIF--
<?php
if (!extension_loaded('gd')) die('skip gd extension not available');
?>
--FILE--
<?php
require_once __DIR__ . DIRECTORY_SEPARATOR . 'func.inc';

$im = imagecreatetruecolor(10, 10);
$white = imagecolorallocate($im, 255, 255, 255);
imagefilledrectangle($im, 0,0, 9,9, $white);
$blue = imagecolorallocate($im, 0, 0, 255);
imagefilledrectangle($im, 3,3, 6,6, $blue);

ob_start();
imagegd($im);
$buffer = ob_get_clean();

$header = unpack('nsignature/nwidth/nheight/Ctruecolor', $buffer);
printf("signature: %d\n", $header['signature']);
printf("truecolor: %d\n", $header['truecolor']);
printf("size: %d\n", strlen($buffer));

test_image_equals_file(__DIR__ . DIRECTORY_SEPARATOR . 'imagegd_truecolor.png', $im);
?>
===DONE===
--EXPECT--
signature: 65534
truecolor: 1
size: 411
The images are equal.
===DONE===
