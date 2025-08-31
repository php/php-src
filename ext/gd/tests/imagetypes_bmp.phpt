--TEST--
imagetypes() - BMP support
--EXTENSIONS--
gd
--FILE--
<?php
var_dump((imagetypes() & IMG_BMP) == function_exists('imagebmp'));
?>
--EXPECT--
bool(true)
