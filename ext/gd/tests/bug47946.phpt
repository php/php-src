--TEST--
Bug #47946 (ImageConvolution overwrites background)
--EXTENSIONS--
gd
--SKIPIF--
<?php
if (!GD_BUNDLED && version_compare(GD_VERSION, '2.2.5', '<=')) die('skip upstream fix not yet released');
if (!(imagetypes() & IMG_PNG)) {
    die("skip No PNG support");
}
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
--EXPECT--
The images are equal.
