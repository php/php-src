--TEST--
Test function gzeof() by calling it more than or less than its expected arguments
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
var_dump(gzeof( $h, $extra_arg ) );
var_dump(gzeof() );
gzclose($h)

?>
===DONE===
--EXPECTF--

Warning: Wrong parameter count for gzeof() in %s on line %d
NULL

Warning: Wrong parameter count for gzeof() in %s on line %d
NULL
===DONE===