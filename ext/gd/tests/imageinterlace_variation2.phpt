--TEST--
Testing setting the interlace bit off with imageinterlace() of GD library
--CREDITS--
Edgar Ferreira da Silva <contato [at] edgarfs [dot] com [dot] br>
#testfest PHPSP on 2009-06-20
--SKIPIF--
<?php 
if (!extension_loaded("gd")) die("skip GD not present");
?>
--FILE--
<?php

$image = imagecreatetruecolor(100, 100);

//setting the interlace bit to on
imageinterlace($image, 1);

//setting de interlace bit to off
var_dump(imageinterlace($image, 0));
var_dump(imageinterlace($image));
?>
--EXPECT--
int(0)
int(0)
