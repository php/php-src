--TEST--
Testing imagegammacorrect() of GD library
--CREDITS--
Rafael Dohms <rdohms [at] gmail [dot] com>
#testfest PHPSP on 2009-06-20
--SKIPIF--
<?php
	if (!extension_loaded("gd")) die("skip GD not present");
	if (!GD_BUNDLED && version_compare(GD_VERSION, '2.2.2', '<')) {
		die("skip test requires GD 2.2.2 or higher");
	}
?>
--FILE--
<?php
$image = imagecreatetruecolor(150, 150);

$grey = imagecolorallocate($image,6,6,6);
$gray = imagecolorallocate($image,15,15,15);

$half =  imagefilledarc ( $image, 75, 75, 70, 70, 0, 180, $grey, IMG_ARC_PIE );
$half2 =  imagefilledarc ( $image, 75, 75, 70, 70, 0, -180, $gray, IMG_ARC_PIE );

$gamma = imagegammacorrect($image, 1, 5);
var_dump((bool) $gamma);

include_once __DIR__ . '/func.inc';
test_image_equals_file(__DIR__ . '/imagegammacorrect_basic.png', $image);
?>
--EXPECT--
bool(true)
The images are equal.
