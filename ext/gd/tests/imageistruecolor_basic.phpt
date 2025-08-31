--TEST--
Testing imageistruecolor() of GD library
--CREDITS--
Rafael Dohms <rdohms [at] gmail [dot] com>
--EXTENSIONS--
gd
--SKIPIF--
<?php
    if (!function_exists("imagecreatetruecolor")) die("skip GD Version not compatible");
?>
--FILE--
<?php
$image = imagecreatetruecolor(180, 30);

var_dump(imageistruecolor($image));
?>
--EXPECT--
bool(true)
