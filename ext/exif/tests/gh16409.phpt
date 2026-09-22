--TEST--
GH-16409 (Segfault in exif_thumbnail when not dealing with a real file)
--EXTENSIONS--
exif
--FILE--
<?php
$stream = popen('echo 1', 'r');
var_dump(exif_thumbnail($stream, $pipes, $stream, $pipes));
?>
--EXPECTF--
Warning: exif_thumbnail(): File too small (0) in %s on line %d
bool(false)%A
