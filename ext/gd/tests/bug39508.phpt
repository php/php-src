--TEST--
Bug #39508 (imagefill crashes with small images 3 pixels or less)
--EXTENSIONS--
gd
--FILE--
<?php
$im = imagecreatetruecolor(3,1);
$bgcolor = imagecolorallocatealpha($im,255, 255, 0, 0);
imagefill($im,0,0,$bgcolor);
print_r(imagecolorat($im, 1,0));
?>
--EXPECT--
16776960
