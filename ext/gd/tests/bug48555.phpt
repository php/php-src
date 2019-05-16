--TEST--
Bug #48555 (ImageFTBBox() differs from previous versions for texts with new lines)
--SKIPIF--
<?php
	if(!extension_loaded('gd')){ die('skip gd extension not available'); }
	if(!function_exists('imageftbbox')) die('skip imageftbbox() not available');
?>
--FILE--
<?php
$cwd = __DIR__;
$font = "$cwd/Tuffy.ttf";
$box = ImageFTBBox(14, 0, $font, "Text without line-break");
//echo 'Top without line-break: ' . $box[7] . "\n";
$without_line_break = $box[7];
$box = ImageFTBBox(14, 0, $font, "Text with\nline-break\none more");
//echo 'Top with line-break: ' . $box[7] . "\n";
$with_line_break = $box[7];

var_dump($without_line_break);
var_dump($with_line_break);
if ($with_line_break==$without_line_break) {
  echo "with line break == without line break".PHP_EOL;
} else {
  echo "with line break != without line break".PHP_EOL;
}

?>
--EXPECTF--
int(-%d)
int(-%d)
with line break == without line break
