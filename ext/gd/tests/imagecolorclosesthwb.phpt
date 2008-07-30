--TEST--
imagecolorclosesthwb() test
--SKIPIF--
<?php
	if(!extension_loaded('gd')){ die('skip: gd extension not available'); }
	if(!function_exists('imagecolorclosesthwb')){ die('skip: imagecolorclosesthwb() not available'); }
?>
--FILE--
<?php
	$im = imagecreatefrompng('test.png');

	echo imagecolorclosesthwb($im, 255, 50, 0);

	imagecolorclosesthwb(NULL);
	imagecolorclosesthwb(NULL, NULL, NULL, NULL);
	imagecolorclosesthwb($im, "hello", "from", "gd");

	imagedestroy($im);
?>
--EXPECTF--
16724480

Warning: imagecolorclosesthwb\(\) expects exactly 4 parameters, 1 given in .* code on line \d+

Warning: imagecolorclosesthwb\(\) expects parameter 1 to be resource, null given in .* code on line \d+

Warning: imagecolorclosesthwb\(\) expects parameter 2 to be long, string given in .* code on line \d+