--TEST--
Bug #42434 (ImageLine w/ antialias = 1px shorter)
--SKIPIF--
<?php
if (!extension_loaded('gd')) {
	die('skip gd extension not available');
}
?>
--FILE--
<?php
$im = imagecreatetruecolor(10, 2);
imagefilledrectangle($im, 0, 0, 10, 2, 0xFFFFFF);

imageantialias($im, true);
imageline($im, 0, 0, 10, 0, 0x000000);

if (imagecolorat($im, 9, 0) == 0x000000) {
	echo 'DONE';
} else {
	echo 'Bugged';
}

imagedestroy($im);
?>
--EXPECTF--
DONE
