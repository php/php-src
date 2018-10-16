--TEST--
Bug #47946 (ImageConvolution overwrites background)
--DESCRIPTION--
The expected image has black pixel artifacts, what is another issue, though
(perhaps #40158).
--SKIPIF--
<?php
if (!extension_loaded('gd')) die('skip gd extension not available');
?>
--FILE--
<?php
function array_flatten($array)
{
    $tempArray = array();

    foreach ($array as $value) {
        if (is_array($value)) {
            $tempArray = array_merge($tempArray, array_flatten($value));
        } else {
            $tempArray[] = $value;
        }
    }

    return $tempArray;
}

function makeFilter($resource, $matrix, $offset = 1.0)
{
    $divisor = array_sum(array_flatten($matrix));
    if ($divisor == 0) {
        $divisor = .01;
    }
    return imageconvolution($resource, $matrix, $divisor, $offset);
}

$edgeMatrix = array(array(1, 0, 1), array(0, 5, 0), array(1, 0, 1));

$im = imagecreatetruecolor(40, 40);
imagealphablending($im, false);
imagefilledrectangle($im, 0, 0, 39, 39, 0x7fffffff);
imagefilledellipse($im, 19, 19, 20, 20, 0x00ff00);
imagesavealpha($im, true);
makeFilter($im, $edgeMatrix);

require_once __DIR__ . '/func.inc';
test_image_equals_file(__DIR__ . '/bug47946_exp.png', $im);
?>
===DONE===
--EXPECT--
The images are equal.
===DONE===
