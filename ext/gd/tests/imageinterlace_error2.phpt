--TEST--
Testing resource that is not a image to imageinterlace() of GD library
--CREDITS--
Edgar Ferreira da Silva <contato [at] edgarfs [dot] com [dot] br>
#testfest PHPSP on 2009-06-20
--SKIPIF--
<?php
if (!extension_loaded("gd")) die("skip GD not present");
?>
--FILE--
<?php
$image = fopen('php://stdin', 'r');
var_dump(imageinterlace($image));
?>
--EXPECTF--
Warning: imageinterlace(): supplied resource is not a valid Image resource in %s on line %d
bool(false)
