--TEST--
Bug #28984 (imagefill segfault using a transparent color).
--SKIPIF--
<?php 
	if (!extension_loaded('gd')) {	
		die("skip gd extension not available\n");
	}
	if (!GD_BUNDLED) {
		die('skip external GD libraries may fail');
	}
?>
--FILE--
<?php
$cwd = dirname(__FILE__);

$im = imagecreatetruecolor(100,100);
imagealphablending($im, false);
#$alpha = imagecolorallocatealpha($im, 255, 0, 255,0);
$alpha = imagecolorallocate($im, 255, 0, 0);
imagefill($im, 2, 2, $alpha);
#imagesavealpha($im, true);
imagepng($im, $cwd . '/1.png');
imagedestroy($im);
@unlink($cwd . '/1.png');
echo "Done\n";
?>
--EXPECT--
Done
