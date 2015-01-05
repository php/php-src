--TEST--
Testing passing no parameters to imageinterlace() of GD library
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

//calling with no parameters
var_dump(imageinterlace());
?>
--EXPECTF--
Warning: imageinterlace() expects at least 1 parameter, 0 given in %s on line %d
NULL
