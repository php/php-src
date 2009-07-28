--TEST--
Bug #48732 (TTF Bounding box wrong for letters below baseline)
--SKIPIF--
<?php
	if(!extension_loaded('gd')){ die('skip gd extension not available'); }
	if(!function_exists('imagefttext')) die('skip imagefttext() not available');
?>
--FILE--
<?php
$cwd = dirname(__FILE__);
$font = "$cwd/Tuffy.ttf";
$g = imagecreate(100, 50);
$bgnd  = imagecolorallocate($g, 255, 255, 255);
$black = imagecolorallocate($g, 0, 0, 0);
$bbox  = imagettftext($g, 12, 0, 0, 20, $black, $font, "ABCEDFGHIJKLMN\nopqrstu\n");
imagepng($g, "$cwd/bug48732.png");
echo 'Left Bottom: (' . $bbox[0]  . ', ' . $bbox[1] . ')';
?>
--CLEAN--
<?php @unlink(dirname(__FILE__) . '/bug48732.png'); ?>
--EXPECTF--
Left Bottom: (0, 47)
