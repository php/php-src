--TEST--
imagecolorclosesthwb() test
--SKIPIF--
<?php
	if(!extension_loaded('gd')){ die('skip: gd extension not available'); }
	if(!function_exists('imagecolorclosesthwb')){ die('skip: imagecolorclosesthwb() not available'); }
?>
--FILE--
<?php
	$im = imagecreatefrompng(dirname(__FILE__).'/test.png');

	var_dump(imagecolorclosesthwb($im, 255, 50, 0));

	var_dump(imagecolorclosesthwb(NULL));
	var_dump(imagecolorclosesthwb(NULL, NULL, NULL, NULL));
	var_dump(imagecolorclosesthwb($im, "hello", "from", "gd"));

	imagedestroy($im);
?>
--EXPECTF--
int(16724480)

Warning: imagecolorclosesthwb() expects exactly 4 parameters, 1 given in %s on line %d
NULL

Warning: imagecolorclosesthwb() expects parameter 1 to be resource, null given in %s on line %d
NULL

Warning: imagecolorclosesthwb() expects parameter 2 to be long, string given in %s on line %d
NULL
