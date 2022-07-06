--TEST--
Bug #73549 (Use after free when stream is passed to imagepng)
--SKIPIF--
<?php
if (!extension_loaded('gd')) die('skip gd extension not available');
?>
--FILE--
<?php
$stream = fopen(__DIR__ . DIRECTORY_SEPARATOR . 'bug73549.png', 'w');
$im = imagecreatetruecolor(8, 8);
var_dump(imagepng($im, $stream));
var_dump($stream);
?>
--EXPECTF--
bool(true)
resource(%d) of type (stream)
--CLEAN--
<?php
unlink(__DIR__ . DIRECTORY_SEPARATOR . 'bug73549.png');
?>
