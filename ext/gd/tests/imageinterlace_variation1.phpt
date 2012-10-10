--TEST--
Testing setting the interlace bit on with imageinterlace() of GD library
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

var_dump(imageinterlace($image, 1));
var_dump(imageinterlace($image));
?>
--EXPECT--
int(1)
int(1)
