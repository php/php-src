--TEST--
Bug #65148 (imagerotate may alter image dimensions)
--EXTENSIONS--
gd
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
}

print_r($results);
?>
--EXPECT--
Array
(
    [IMG_BELL] => Array
        (
            [x] => 42
            [y] => 24
        )

    [IMG_BESSEL] => Array
        (
            [x] => 42
            [y] => 24
        )

    [IMG_BICUBIC] => Array
        (
            [x] => 42
            [y] => 24
        )

    [IMG_BICUBIC_FIXED] => Array
        (
            [x] => 42
            [y] => 24
        )

    [IMG_BILINEAR_FIXED] => Array
        (
            [x] => 42
            [y] => 24
        )

    [IMG_BLACKMAN] => Array
        (
            [x] => 42
            [y] => 24
        )

    [IMG_BOX] => Array
        (
            [x] => 42
            [y] => 24
        )

    [IMG_BSPLINE] => Array
        (
            [x] => 42
            [y] => 24
        )

    [IMG_CATMULLROM] => Array
        (
            [x] => 42
            [y] => 24
        )

    [IMG_GAUSSIAN] => Array
        (
            [x] => 42
            [y] => 24
        )

    [IMG_GENERALIZED_CUBIC] => Array
        (
            [x] => 42
            [y] => 24
        )

    [IMG_HERMITE] => Array
        (
            [x] => 42
            [y] => 24
        )

    [IMG_HAMMING] => Array
        (
            [x] => 42
            [y] => 24
        )

    [IMG_HANNING] => Array
        (
            [x] => 42
            [y] => 24
        )

    [IMG_MITCHELL] => Array
        (
            [x] => 42
            [y] => 24
        )

    [IMG_POWER] => Array
        (
            [x] => 42
            [y] => 24
        )

    [IMG_QUADRATIC] => Array
        (
            [x] => 42
            [y] => 24
        )

    [IMG_SINC] => Array
        (
            [x] => 42
            [y] => 24
        )

    [IMG_NEAREST_NEIGHBOUR] => Array
        (
            [x] => 42
            [y] => 24
        )

    [IMG_WEIGHTED4] => Array
        (
            [x] => 42
            [y] => 24
        )

    [IMG_TRIANGLE] => Array
        (
            [x] => 42
            [y] => 24
        )

)
