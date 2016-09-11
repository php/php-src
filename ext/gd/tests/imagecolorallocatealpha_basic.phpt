--TEST--
Testing imagecolorallocatealpha()
--CREDITS--
Rafael Dohms <rdohms [at] gmail [dot] com>
--SKIPIF--
<?php 
	if (!extension_loaded("gd")) die("skip GD not present");
	if (!GD_BUNDLED && version_compare(GD_VERSION, '2.2.2', '<')) {
		die("skip test requires GD 2.2.2 or higher");
	}
?>
--FILE--
<?php
$img = imagecreatetruecolor(150, 150);

$cor = imagecolorallocate($img, 50, 100, 255);
$corA = imagecolorallocatealpha($img, 50, 100, 255, 50);
//$whiteA = imagecolorallocatealpha($img, 255, 255, 255, 127);

$half =  imagefilledarc ( $img, 75, 75, 70, 70, 0, 180, $cor, IMG_ARC_PIE );
$half2 =  imagefilledarc ( $img, 75, 75, 70, 70, 180, 360, $corA, IMG_ARC_PIE );

include_once __DIR__ . '/func.inc';
test_image_equals_file(__DIR__ . '/imagecolorallocatealpha_basic.png', $img);
var_dump($corA);
?>
--EXPECT--
The images are equal.
int(842163455)