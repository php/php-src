--TEST--
Testing imagegammacorrect() of GD library with non TrueColor image
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
$image = imagecreate(150, 150);

$grey = imagecolorallocate($image,6,6,6);
$gray = imagecolorallocate($image,15,15,15);

$half =  imagefilledarc ( $image, 75, 75, 70, 70, 0, 180, $grey, IMG_ARC_PIE );
$half2 =  imagefilledarc ( $image, 75, 75, 70, 70, 0, -180, $gray, IMG_ARC_PIE );

$gamma = imagegammacorrect($image, 1, 5);

if ($gamma){
	ob_start();
	imagegd($image);
	$img = ob_get_contents();
	ob_end_clean();
}

echo md5(base64_encode($img));
?>
--EXPECT--
32fdb7b3675e4a9fd601c6a0645c88b3
