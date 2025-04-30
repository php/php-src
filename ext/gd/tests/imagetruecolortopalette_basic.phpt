--TEST--
Testing imagetruecolortopalette() of GD library
--CREDITS--
Rafael Dohms <rdohms [at] gmail [dot] com>
--EXTENSIONS--
gd
--SKIPIF--
<?php
    if (!GD_BUNDLED && version_compare(GD_VERSION, '2.2.2', '<')) {
        die("skip test requires GD 2.2.2 or higher");
    }
    if (!function_exists("imagecreatetruecolor")) die("skip GD Version not compatible");
    if (!(imagetypes() & IMG_PNG)) {
        die("skip No PNG support");
    }
?>
--FILE--
<?php
$image = imagecreatetruecolor(150, 150);

$a = imagecolorallocate($image,255,0,255);
$b = imagecolorallocate($image,0,255,255);

$half =  imagefilledarc ( $image, 75, 75, 70, 70, 0, 180, $a, IMG_ARC_PIE );
$half2 =  imagefilledarc ( $image, 75, 55, 80, 70, 0, -180, $b, IMG_ARC_PIE );

var_dump(imagetruecolortopalette($image, true, 2));

include_once __DIR__ . '/func.inc';
test_image_equals_file(__DIR__ . '/imagetruecolortopalette_basic.png', $image);
?>
--EXPECT--
bool(true)
The images are equal.
