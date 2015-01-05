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
Warning: imagecopymerge() expects exactly 9 parameters, 0 given in %s on line %d
