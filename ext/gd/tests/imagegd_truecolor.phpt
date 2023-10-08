--TEST--
imagegd() writes truecolor images without palette conversion
--EXTENSIONS--
gd
--SKIPIF--
<?php
    if (!GD_BUNDLED && version_compare(GD_VERSION, '2.3.3', '>=')) {
        die("skip test requires GD 2.3.2 or older");
    }
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
--EXPECT--
signature: 65534
truecolor: 1
size: 411
The images are equal.
