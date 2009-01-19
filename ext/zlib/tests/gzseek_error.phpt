--TEST--
Test function gzseek() by calling it more than or less than its expected arguments
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
$offset = 1;
$whence = SEEK_SET;
$extra_arg = 'nothing'; 

var_dump(gzseek( $h, $offset, $whence, $extra_arg ) );
var_dump(gzseek($h));
var_dump(gzseek());

?>
===DONE===
--EXPECTF--

Warning: Wrong parameter count for gzseek() in %s on line %d
NULL

Warning: Wrong parameter count for gzseek() in %s on line %d
NULL

Warning: Wrong parameter count for gzseek() in %s on line %d
NULL
===DONE===