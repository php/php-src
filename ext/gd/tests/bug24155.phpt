--TEST--
Bug #24155 (gdImageRotate270 rotation problem).
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
	$dest = dirname(realpath(__FILE__)) . '/bug24155.png';
	@unlink($dest);

	$im = imagecreatetruecolor(30, 50);
	imagefill($im, 0, 0, (16777215 - 255)); 
	$im = imagerotate($im, 270, 255);
	imagepng($im, $dest);

	echo md5_file($dest) . "\n";	
	@unlink($dest);
?>
--EXPECT--
cc867fd65c30883463ce58d0341f0997
