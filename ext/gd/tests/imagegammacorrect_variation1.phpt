--TEST--
Testing imagegammacorrect() of GD library with non TrueColor image
--CREDITS--
Rafael Dohms <rdohms [at] gmail [dot] com>
#testfest PHPSP on 2009-06-20
--SKIPIF--
<?php 
	if (!extension_loaded("gd")) die("skip GD not present");
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
	imagepng($image, null, 9);
	$img = ob_get_contents();
	ob_end_clean();
}

echo md5(base64_encode($img));
?>
--EXPECT--
b017b1ddc8bda00e82aa8cbfb54c35d4
