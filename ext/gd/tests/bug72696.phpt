--TEST--
Bug #72696 (imagefilltoborder stackoverflow on truecolor images)
--SKIPIF--
<?php
if (!extension_loaded('gd')) die('skip gd extension not available');
?>
--FILE--
<?php
$im = imagecreatetruecolor(10, 10);
imagefilltoborder($im, 0, 0, 1, -2);
?>
===DONE===
--EXPECT--
===DONE===
