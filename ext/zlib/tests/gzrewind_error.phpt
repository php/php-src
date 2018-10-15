--TEST--
Test function gzrewind() by calling it more than or less than its expected arguments
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
var_dump(gzrewind( $h, $extra_arg ) );
var_dump(gzrewind());
gzclose($h);

?>
===DONE===
--EXPECTF--
Warning: gzrewind() expects exactly 1 parameter, 2 given in %s on line %d
bool(false)

Warning: gzrewind() expects exactly 1 parameter, 0 given in %s on line %d
bool(false)
===DONE===
