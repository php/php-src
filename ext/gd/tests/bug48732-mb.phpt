--TEST--
Bug #48732 (TTF Bounding box wrong for letters below baseline)
--SKIPIF--
<?php
	if(!extension_loaded('gd')){ die('skip gd extension not available'); }
	if(!function_exists('imagefttext')) die('skip imagefttext() not available');
	if (substr(PHP_OS, 0, 3) == 'WIN') die('skip UTF-8 font file names not yet supported on Windows');
?>
--FILE--
<?php
$cwd = dirname(__FILE__);
$font = "$cwd/Tuffy私はガラスを食べられます.ttf";
$g = imagecreate(100, 50);
$bgnd  = imagecolorallocate($g, 255, 255, 255);
$black = imagecolorallocate($g, 0, 0, 0);
$bbox  = imagettftext($g, 12, 0, 0, 20, $black, $font, "ABCEDFGHIJKLMN\nopqrstu\n");
imagepng($g, "$cwd/bug48732私はガラスを食べられます.png");
echo 'Left Bottom: (' . $bbox[0]  . ', ' . $bbox[1] . ')';
?>
--CLEAN--
<?php @unlink(dirname(__FILE__) . '/bug48732私はガラスを食べられます.png'); ?>
--EXPECTF--
Left Bottom: (0, 46)
