--TEST--
Bug #48555 (ImageFTBBox() differs from previous versions for texts with new lines)
--SKIPIF--
<?php
	if(!extension_loaded('gd')){ die('skip gd extension not available'); }
	if(!function_exists('imageftbbox')) die('skip imageftbbox() not available');
?>
--FILE--
<?php
$cwd = dirname(__FILE__);
$font = "$cwd/Tuffy.ttf";
$box = ImageFTBBox(14, 0, $font, "Text without line-break");
echo 'Top without line-break: ' . $box[7] . "\n";
$box = ImageFTBBox(14, 0, $font, "Text with\nline-break\none more");
echo 'Top with line-break: ' . $box[7] . "\n";
?>
--EXPECTF--
Top without line-break: -14
Top with line-break: -14
