--TEST--
Test function gzclose() by calling it more than or less than its expected arguments
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



var_dump(gzclose( $h, $extra_arg ) );
var_dump(gzclose());

gzclose($h);


?>
===DONE===
--EXPECTF--

Warning: Wrong parameter count for gzclose() in %s on line %d
NULL

Warning: Wrong parameter count for gzclose() in %s on line %d
NULL
===DONE===