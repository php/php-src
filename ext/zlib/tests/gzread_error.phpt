--TEST--
Test function gzread() by calling it more than or less than its expected arguments
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

var_dump(gzread( $h, $length, $extra_arg ) );

var_dump(gzread());

gzclose($h);

?>
===DONE===
--EXPECTF--
Warning: gzread() expects exactly 2 parameters, 3 given in %s on line %d
bool(false)

Warning: gzread() expects exactly 2 parameters, 0 given in %s on line %d
bool(false)
===DONE===
