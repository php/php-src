--TEST--
Test imagesetbrush() function : basic functionality
--CREDITS--
Erick Belluci Tedeschi <erickbt86 [at] gmail [dot] com>
#testfest PHPSP on 2009-06-20
--SKIPIF--
<?php
if (!extension_loaded('gd')) {
	die('skip gd extension is not loaded');
}
?>
--FILE--
<?php
// Create the brush image
$img = imagecreate(10, 10);
 
// Create the main image, 100x100
$mainimg = imagecreatetruecolor(100, 100);
  
$white = imagecolorallocate($img, 255, 0, 0);
imagefilledrectangle($img, 0, 0, 299, 99, $white);

// Set the brush
imagesetbrush($mainimg, $img);
   
// Draw a couple of brushes, each overlaying each
imageline($mainimg, 50, 50, 50, 60, IMG_COLOR_BRUSHED);

// Get output and generate md5 hash
ob_start();
imagepng($mainimg, null, 9);
$result_image = ob_get_contents();
ob_end_clean();
echo md5(base64_encode($result_image));
?>
--EXPECT--
8168577c0d1fe6d9d11397cb15263d82
