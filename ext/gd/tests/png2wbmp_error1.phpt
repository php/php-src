--TEST--
Test png2wbmp() function : wrong threshold value param
--CREDITS--
Levi Fukumori <levi [at] fukumori [dot] com [dot] br>
#testfest PHPSP on 2009-06-20
--SKIPIF--
<?php
if(!extension_loaded('gd')) {
    die('skip gd extension is not loaded');
}
if(!function_exists('png2wbmp')) {
    die('skip png2wbmp function is not available');
}
?>
--FILE--
<?php
// Create a blank image and add some text
$im = imagecreatetruecolor(120, 20);
$text_color = imagecolorallocate($im, 255, 255, 255);
imagestring($im, 1, 5, 5,  'A Simple Text String', $text_color);

$file = __DIR__ .'/simpletext.png';
$file2 = __DIR__ .'/simpletext.wbmp';

// Save the image as 'simpletext.png'
imagepng($im, $file);

// Free up memory
imagedestroy($im);

png2wbmp($file, $file2, 20, 120, 9);
png2wbmp($file, $file2, 20, 120, -1);
?>
--EXPECTF--
Deprecated: Function png2wbmp() is deprecated in %s on line %d

Warning: png2wbmp(): Invalid threshold value '9' in %s on line %d

Deprecated: Function png2wbmp() is deprecated in %s on line %d

Warning: png2wbmp(): Invalid threshold value '-1' in %s on line %d
--CLEAN--
<?php
unlink(__DIR__ .'/simpletext.png');
unlink(__DIR__ .'/simpletext.wbmp');
?>
