--TEST--
Test function gzgets() by calling it more than or less than its expected arguments
--SKIPIF--
<?php
if (!extension_loaded("zlib")) {
	print "skip - ZLIB extension not loaded";
}
?>
--FILE--
<?php

$f = dirname(__FILE__)."/004.txt.gz";
$h = gzopen($f, 'r');
$length = 10;
$extra_arg = 'nothing';
var_dump(gzgets( $h, $length, $extra_arg ) );

var_dump(gzgets());


?>
===DONE===
--EXPECTF--
Warning: gzgets() expects at most 2 parameters, 3 given in %s on line %d
bool(false)

Warning: gzgets() expects at least 1 parameter, 0 given in %s on line %d
bool(false)
===DONE===
