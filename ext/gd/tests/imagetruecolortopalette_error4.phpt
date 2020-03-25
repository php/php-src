--TEST--
Testing imagetruecolortopalette(): out of range parameter 3
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
    fn() => imagetruecolortopalette($image, true, 0),
    fn() => imagetruecolortopalette($image, true, -1)
);

?>
--EXPECT--
!! [ValueError] Number of colors has to be greater than zero and no more than 2147483647
!! [ValueError] Number of colors has to be greater than zero and no more than 2147483647
