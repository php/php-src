--TEST--
Testing setting the interlace bit on with imageinterlace() of GD library
--CREDITS--
Edgar Ferreira da Silva <contato [at] edgarfs [dot] com [dot] br>
#testfest PHPSP on 2009-06-20
--EXTENSIONS--
gd
--FILE--
<?php

$image = imagecreatetruecolor(100, 100);

var_dump(imageinterlace($image, true));
var_dump(imageinterlace($image));
?>
--EXPECT--
bool(true)
bool(true)
