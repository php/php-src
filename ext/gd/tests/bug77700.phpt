--TEST--
Bug #77700 (Writing truecolor images as GIF ignores interlace flag)
--SKIPIF--
<?php
if (!extension_loaded('gd')) die('skip gd extension not available');
?>
--FILE--
<?php
$im = imagecreatetruecolor(10, 10);
imagefilledrectangle($im, 0, 0, 9, 9, imagecolorallocate($im, 255, 255, 255));
imageinterlace($im, 1);
imagegif($im, __DIR__ . 'bug77700.gif');

$im = imagecreatefromgif(__DIR__ . 'bug77700.gif');
var_dump(imageinterlace($im));
?>
===DONE===
--EXPECT--
int(1)
===DONE===
--CLEAN--
<?php
unlink(__DIR__ . 'bug77700.gif');
?>
