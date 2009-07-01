--TEST--
Testing wrong parameter passing imagecopymerge() of GD library
--CREDITS--
Cleston Viel Vieira de Sousa <cleston [dot] vs [at] gmail [dot] com>
#testfest PHPSP on 2009-06-20
--SKIPIF--
<?php 
if (!extension_loaded("gd")) die("skip GD not present");
?>
--FILE--
<?php

imagecopymerge();

?>
--EXPECTF--
Warning: Wrong parameter count for imagecopymerge() in %s on line %d
