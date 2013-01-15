--TEST--
Bug #48801 (Problem with imagettfbbox) freetype < 2.4.10
--SKIPIF--
<?php
	if(!extension_loaded('gd')){ die('skip gd extension not available'); }
	if(!function_exists('imageftbbox')) die('skip imageftbbox() not available');

	include dirname(__FILE__) . '/func.inc';
	if(version_compare(get_freetype_version(), '2.4.10') >= 0) die('skip for freetype < 2.4.10');
?>
--FILE--
<?php
$cwd = dirname(__FILE__);
$font = "$cwd/Tuffy.ttf";
$bbox = imageftbbox(50, 0, $font, "image");
echo '(' . $bbox[0] . ', ' . $bbox[1] . ")\n";
echo '(' . $bbox[2] . ', ' . $bbox[3] . ")\n";
echo '(' . $bbox[4] . ', ' . $bbox[5] . ")\n";
echo '(' . $bbox[6] . ', ' . $bbox[7] . ")\n";
?>
--EXPECTF--
(-1, 15)
(155, 15)
(155, -48)
(-1, -48)
