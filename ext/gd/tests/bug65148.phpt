--TEST--
Bug #65148 (imagerotate may alter image dimensions)
--SKIPIF--
<?php
if (!extension_loaded('gd')) die('skip gd extension is not available');
?>
--FILE--
<?php

$interpolations = array(
    'IMG_BELL' => IMG_BELL,
    'IMG_BESSEL' => IMG_BESSEL,
    'IMG_BICUBIC' => IMG_BICUBIC,
    'IMG_BICUBIC_FIXED' => IMG_BICUBIC_FIXED,
    'IMG_BILINEAR_FIXED' => IMG_BILINEAR_FIXED,
    'IMG_BLACKMAN' => IMG_BLACKMAN,
    'IMG_BOX' => IMG_BOX,
    'IMG_BSPLINE' => IMG_BSPLINE,
    'IMG_CATMULLROM' => IMG_CATMULLROM,
    'IMG_GAUSSIAN' => IMG_GAUSSIAN,
    'IMG_GENERALIZED_CUBIC' => IMG_GENERALIZED_CUBIC,
    'IMG_HERMITE' => IMG_HERMITE,
    'IMG_HAMMING' => IMG_HAMMING,
    'IMG_HANNING' => IMG_HANNING,
    'IMG_MITCHELL' => IMG_MITCHELL,
    'IMG_POWER' => IMG_POWER,
    'IMG_QUADRATIC' => IMG_QUADRATIC,
    'IMG_SINC' => IMG_SINC,
    'IMG_NEAREST_NEIGHBOUR' => IMG_NEAREST_NEIGHBOUR,
    'IMG_WEIGHTED4' => IMG_WEIGHTED4,
    'IMG_TRIANGLE' => IMG_TRIANGLE,
);

$img = imagecreate(40, 20);
$results = array();

foreach ($interpolations as $name => $interpolation) {
  imagesetinterpolation($img, $interpolation);
  $t = imagecolorallocatealpha($img, 0, 0, 0, 127);
  $imgr = imagerotate($img, -5, $t);
  $results[$name] = array('x' => imagesx($imgr), 'y' => imagesy($imgr));
  imagedestroy($imgr);
}

imagedestroy($img);
print_r($results);
?>
--EXPECT--
Array
(
    [IMG_BELL] => Array
        (
            [x] => 40
            [y] => 23
        )

    [IMG_BESSEL] => Array
        (
            [x] => 40
            [y] => 23
        )

    [IMG_BICUBIC] => Array
        (
            [x] => 40
            [y] => 23
        )

    [IMG_BICUBIC_FIXED] => Array
        (
            [x] => 40
            [y] => 23
        )

    [IMG_BILINEAR_FIXED] => Array
        (
            [x] => 40
            [y] => 23
        )

    [IMG_BLACKMAN] => Array
        (
            [x] => 40
            [y] => 23
        )

    [IMG_BOX] => Array
        (
            [x] => 40
            [y] => 23
        )

    [IMG_BSPLINE] => Array
        (
            [x] => 40
            [y] => 23
        )

    [IMG_CATMULLROM] => Array
        (
            [x] => 40
            [y] => 23
        )

    [IMG_GAUSSIAN] => Array
        (
            [x] => 40
            [y] => 23
        )

    [IMG_GENERALIZED_CUBIC] => Array
        (
            [x] => 40
            [y] => 23
        )

    [IMG_HERMITE] => Array
        (
            [x] => 40
            [y] => 23
        )

    [IMG_HAMMING] => Array
        (
            [x] => 40
            [y] => 23
        )

    [IMG_HANNING] => Array
        (
            [x] => 40
            [y] => 23
        )

    [IMG_MITCHELL] => Array
        (
            [x] => 40
            [y] => 23
        )

    [IMG_POWER] => Array
        (
            [x] => 40
            [y] => 23
        )

    [IMG_QUADRATIC] => Array
        (
            [x] => 40
            [y] => 23
        )

    [IMG_SINC] => Array
        (
            [x] => 40
            [y] => 23
        )

    [IMG_NEAREST_NEIGHBOUR] => Array
        (
            [x] => 40
            [y] => 23
        )

    [IMG_WEIGHTED4] => Array
        (
            [x] => 40
            [y] => 23
        )

    [IMG_TRIANGLE] => Array
        (
            [x] => 40
            [y] => 23
        )

)
