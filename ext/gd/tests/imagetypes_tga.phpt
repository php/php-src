--TEST--
imagetypes() - TGA support
--EXTENSIONS--
gd
--FILE--
<?php
var_dump((imagetypes() & IMG_TGA) == function_exists('imagecreatefromtga'));
?>
--EXPECT--
bool(true)
