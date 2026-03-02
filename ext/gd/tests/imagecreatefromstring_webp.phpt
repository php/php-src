--TEST--
imagecreatefromstring() - WEBP format
--EXTENSIONS--
gd
--SKIPIF--
<?php
if (!(imagetypes() & IMG_WEBP)) die('skip WEBP support required');
?>
--FILE--
<?php
// create an image from a WEBP string representation
$im = imagecreatefromstring(file_get_contents(__DIR__ . '/imagecreatefromstring.webp'));
var_dump(imagesx($im));
var_dump(imagesy($im));

?>
--EXPECT--
int(10)
int(10)
