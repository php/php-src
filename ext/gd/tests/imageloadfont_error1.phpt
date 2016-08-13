--TEST--
Testing that imageloadfont() breaks on non-string first parameter
--CREDITS--
Neveo Harrison <neveoo [at] gmail [dot] com> #testfest #tek11
--SKIPIF--
<?php
	if (!extension_loaded("gd")) die("skip GD not present");
?>
--FILE--
<?php
var_dump( imageloadfont(array()) );
?>
--EXPECTF--
Warning: imageloadfont() expects parameter 1 to be a valid path, array given in %s on line %d
NULL
