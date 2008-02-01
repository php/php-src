--TEST--
imagecopyresampled()
--SKIPIF--
<?php 
	if (!function_exists('imagecopyresampled')) die('skip imagecopyresampled() not available'); 
	require_once('skipif_imagetype.inc');
?>
--FILE--
<?php

/* Prototype  : bool imagecopyresampled  ( resource $dst_image  , resource $src_image  , int $dst_x  , int $dst_y  , int $src_x  , int $src_y  , int $dst_w  , int $dst_h  , int $src_w  , int $src_h  )
 * Description: Copy and resize part of an image with resampling.
 * Source code: ext/standard/image.c
 * Alias to functions: 
 */

echo "Simple test of imagecopyresampled() function\n";

$dest_lge = dirname(realpath(__FILE__)) . '/imagelarge.png';
$dest_sml = dirname(realpath(__FILE__)) . '/imagesmall.png';

// create a blank image
$image_lge = imagecreatetruecolor(400, 300);

// set the background color to black 
$bg = imagecolorallocate($image_lge, 0, 0, 0);

// fill polygon with blue
$col_ellipse = imagecolorallocate($image_lge, 0, 255, 0);

// draw the eclipse
imagefilledellipse($image_lge, 200, 150, 300, 200, $col_ellipse);

// output the picture to a file
imagepng($image_lge, $dest_lge);

// Get new dimensions
$percent = 0.5; // new image 50% of orginal
list($width, $height) = getimagesize($dest_lge);
echo "Size of orginal: width=". $width . " height=" . $height . "\n";

$new_width = $width * $percent;
$new_height = $height * $percent;

// Resample
$image_sml = imagecreatetruecolor($new_width, $new_height);
imagecopyresampled($image_sml, $image_lge, 0, 0, 0, 0, $new_width, $new_height, $width, $height);

imagepng($image_sml, $dest_sml);

list($width, $height) = getimagesize($dest_sml);
echo "Size of copy: width=". $width . " height=" . $height . "\n";

imagedestroy($image_lge); 
imagedestroy($image_sml);
 

echo "Done\n"; 
?>
--CLEAN--
<?php 
	$dest_lge = dirname(realpath(__FILE__)) . '/imagelarge.png';
	$dest_sml = dirname(realpath(__FILE__)) . '/imagesmall.png';
	@unlink($dest_lge);
	@unlink($dest_sml);
?>
--EXPECT--
Simple test of imagecopyresampled() function
Size of orginal: width=400 height=300
Size of copy: width=200 height=150
Done
