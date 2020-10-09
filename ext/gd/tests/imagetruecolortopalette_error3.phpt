--TEST--
Testing imagetruecolortopalette(): wrong parameters for parameter 3
--CREDITS--
Rafael Dohms <rdohms [at] gmail [dot] com>
--SKIPIF--
<?php
    if (!extension_loaded("gd")) die("skip GD not present");
    if (!function_exists("imagecreatetruecolor")) die("skip GD Version not compatible");
?>
--FILE--
<?php
require __DIR__  . '/func.inc';

$image = imagecreatetruecolor(50, 50);

trycatch_dump(
    fn() => imagetruecolortopalette($image, true, null)
);

?>
--EXPECT--
!! [ValueError] imagetruecolortopalette(): Argument #3 ($num_colors) must be greater than 0 and less than 2147483647
