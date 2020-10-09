--TEST--
Testing imagecolordeallocate() of GD library with Out of range integers (Below)
--CREDITS--
Rafael Dohms <rdohms [at] gmail [dot] com>
#testfest PHPSP on 2009-06-20
--SKIPIF--
<?php
    if (!extension_loaded("gd")) die("skip GD not present");
?>
--FILE--
<?php

require_once __DIR__ . '/func.inc';
$image = imagecreate(180, 30);
$white = imagecolorallocate($image, 255, 255, 255);

$totalColors = imagecolorstotal($image);

trycatch_dump(
    fn() => imagecolordeallocate($image, -1.0)
);

?>
--EXPECT--
!! [ValueError] imagecolordeallocate(): Argument #2 ($color) must be between 0 and 1
