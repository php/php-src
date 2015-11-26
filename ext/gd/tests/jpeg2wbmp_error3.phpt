--TEST--
Test jpeg2wbmp() function : wrong destination filename param
--CREDITS--
Levi Fukumori <levi [at] fukumori [dot] com [dot] br>
#testfest PHPSP on 2009-06-20
--SKIPIF--
<?php 
if(!extension_loaded('gd')) {
    die('skip gd extension is not loaded');
}
if(!function_exists('jpeg2wbmp')) {
    die('skip jpeg2wbmp function is not available');
}
?>
--FILE--
<?php
// Create a blank image and add some text
$im = imagecreatetruecolor(120, 20);
$text_color = imagecolorallocate($im, 255, 255, 255);
imagestring($im, 1, 5, 5,  'A Simple Text String', $text_color);

$file = dirname(__FILE__) .'/simpletext.jpg';

// Save the image as 'simpletext.jpg'
imagejpeg($im, $file);

// Free up memory
imagedestroy($im);

jpeg2wbmp($file, '', 20, 120, 8);
jpeg2wbmp($file, null, 20, 120, 8);
jpeg2wbmp($file, false, 20, 120, 8);
?>
--EXPECTF--
Warning: jpeg2wbmp(): Unable to open '' for writing in %s on line %d

Warning: jpeg2wbmp(): Unable to open '' for writing in %s on line %d

Warning: jpeg2wbmp(): Unable to open '' for writing in %s on line %d
--CLEAN--
<?php
unlink(dirname(__FILE__) .'/simpletext.jpg');
?>
