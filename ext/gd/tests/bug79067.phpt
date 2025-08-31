--TEST--
Bug #79067 (gdTransformAffineCopy() may use unitialized values)
--EXTENSIONS--
gd
--FILE--
<?php
$matrix = [1, 1, 1, 1, 1, 1];
$src = imagecreatetruecolor(8, 8);
var_dump(imageaffine($src, $matrix));
?>
--EXPECT--
bool(false)
