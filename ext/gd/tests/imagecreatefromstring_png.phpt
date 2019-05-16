--TEST--
imagecreatefromstring() - PNG format
--SKIPIF--
<?php
if (!extension_loaded('gd')) die('skip ext/gd required');
if (!(imagetypes() & IMG_PNG)) die('skip PNG support required');
?>
--FILE--
<?php
// create an image from a PNG string representation
$im = imagecreatefromstring(file_get_contents(__DIR__ . '/imagecreatefromstring.gif'));
var_dump(imagesx($im));
var_dump(imagesy($im));

?>
===DONE===
--EXPECT--
int(10)
int(10)
===DONE===
