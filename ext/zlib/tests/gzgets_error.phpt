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

Warning: Wrong parameter count for gzgets() in %s on line %d
NULL

Warning: Wrong parameter count for gzgets() in %s on line %d
NULL
===DONE===