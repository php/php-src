--TEST--
Test function gzpassthru() by calling it more than or less than its expected arguments
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
var_dump(gzpassthru( $h, $extra_arg ) );
var_dump(gzpassthru() );
gzclose($h);

?>
===DONE===
--EXPECTF--

Warning: Wrong parameter count for gzpassthru() in %s on line %d
NULL

Warning: Wrong parameter count for gzpassthru() in %s on line %d
NULL
===DONE===