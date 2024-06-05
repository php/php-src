--TEST--
imagecolorset() parameters errors
--EXTENSIONS--
gd
--FILE--
<?php

require __DIR__ . '/func.inc';

$im = imagecreate(5, 5);

$c = imagecolorallocatealpha($im, 3, 4, 5, 6);

trycatch_dump(
    fn() => imagecolorset($im, $c, -3, 4, 5, 6),
    fn() => imagecolorset($im, $c, 3, -4, 5, 6),
    fn() => imagecolorset($im, $c, 3, 4, -5, 6),
    fn() => imagecolorset($im, $c, 3, 4, 5, -6),
);

?>
--EXPECT--
!! [ValueError] imagecolorset(): Argument #3 ($red) must be between 0 and 255 (inclusive)
!! [ValueError] imagecolorset(): Argument #4 ($green) must be between 0 and 255 (inclusive)
!! [ValueError] imagecolorset(): Argument #5 ($blue) must be between 0 and 255 (inclusive)
NULL
