--TEST--
Test function gzgetc() by calling it more than or less than its expected arguments
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
$extra_arg = 'nothing';
var_dump(gzgetc( $h, $extra_arg ) );

var_dump(gzgetc() );

gzclose($h);
?>
===DONE===
--EXPECTF--

Warning: gzgetc() expects exactly 1 parameter, 2 given in %s on line %d
bool(false)

Warning: gzgetc() expects exactly 1 parameter, 0 given in %s on line %d
bool(false)
===DONE===