--TEST--
Testing setting the interlace bit off with imageinterlace() of GD library
--CREDITS--
Edgar Ferreira da Silva <contato [at] edgarfs [dot] com [dot] br>
#testfest PHPSP on 2009-06-20
--EXTENSIONS--
gd
--FILE--
<?php

$image = imagecreatetruecolor(100, 100);

//setting the interlace bit to on
imageinterlace($image, true);

//setting de interlace bit to off
var_dump(imageinterlace($image, false));
var_dump(imageinterlace($image));
?>
--EXPECT--
bool(false)
bool(false)
