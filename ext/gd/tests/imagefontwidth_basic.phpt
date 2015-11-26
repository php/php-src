--TEST--
Testing imagefontwidth() of GD library
--CREDITS--
Rafael Dohms <rdohms [at] gmail [dot] com>
#testfest PHPSP on 2009-06-20
--SKIPIF--
<?php 
	if (!extension_loaded("gd")) die("skip GD not present");
?>
--FILE--
<?php
var_dump(imagefontwidth(1),imagefontwidth(2),imagefontwidth(3),imagefontwidth(4),imagefontwidth(5));
var_dump(imagefontwidth(1) < imagefontwidth(5));
?>
--EXPECT--
int(5)
int(6)
int(7)
int(8)
int(9)
bool(true)
