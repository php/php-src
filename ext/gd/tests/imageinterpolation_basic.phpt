--TEST--
imagegetinterpolation() and imagesetinterpolation() basic test
--EXTENSIONS--
gd
--FILE--
<?php
$methods = array(
    IMG_BELL,
    IMG_BESSEL,
    IMG_BILINEAR_FIXED,
    IMG_BICUBIC,
    IMG_BICUBIC_FIXED,
    IMG_BLACKMAN,
    IMG_BOX,
    IMG_BSPLINE,
    IMG_CATMULLROM,
    IMG_GAUSSIAN,
    IMG_GENERALIZED_CUBIC,
    IMG_HERMITE,
    IMG_HAMMING,
    IMG_HANNING,
    IMG_MITCHELL,
    IMG_NEAREST_NEIGHBOUR,
    IMG_POWER,
    IMG_QUADRATIC,
    IMG_SINC,
    IMG_TRIANGLE,
    IMG_WEIGHTED4,
);
$im = imagecreate(8, 8);
foreach ($methods as $method) {
    imagesetinterpolation($im, $method);
    var_dump(imagegetinterpolation($im) === $method);
}
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
