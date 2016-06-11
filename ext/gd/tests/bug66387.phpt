--TEST--
Bug #66387 (Stack overflow with imagefilltoborder)
--SKIPIF--
<?php
if (!extension_loaded('gd')) die('skip gd extension not available!');
?>
--FILE--
<?php
$im = imagecreatetruecolor(20, 20);
$c = imagecolorallocate($im, 255, 0, 0);
imagefilltoborder($im, 0, -999355, $c, $c);
echo "ready\n";
?>
--EXPECT--
ready
