--TEST--
Bug #48555 (ImageFTBBox() differs from previous versions for texts with new lines)
--SKIPIF--
<?php
	if(!extension_loaded('gd')){ die('skip gd extension not available'); }
?>
--FILE--
<?php
$cwd = dirname(__FILE__);
$font = "$cwd/Tuffy.ttf";
$box = ImageFTBBox(13, 0, $font, "Text without line-break");
echo 'Top without line-break: ' . $box[7] . "\n";
$box = ImageFTBBox(13, 0, $font, "Text with\nline-break\none more");
echo 'Top with line-break: ' . $box[7] . "\n";
?>
--EXPECTF--
Top without line-break: -15
Top with line-break: -15
