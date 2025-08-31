--TEST--
imagecreatefromstring() - WBMP format
--EXTENSIONS--
gd
--SKIPIF--
<?php
if (!(imagetypes() & IMG_WBMP)) die('skip GIF support required');
?>
--FILE--
<?php
// create an image from a WBMP string representation
$im = imagecreatefromstring(file_get_contents(__DIR__ . '/imagecreatefromstring.wbmp'));
var_dump(imagesx($im));
var_dump(imagesy($im));

?>
--EXPECT--
int(200)
int(200)
