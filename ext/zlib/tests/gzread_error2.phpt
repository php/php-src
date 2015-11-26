--TEST--
Test function gzread() by calling it invalid lengths
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
var_dump(gzread($h, 10));
var_dump(gzread($h, 0));
var_dump(gzread($h, 5));
var_dump(gzread($h, -1));
var_dump(gzread($h, 8));
gzclose($h);

?>
===DONE===
--EXPECTF--
string(10) "When you'r"

Warning: gzread(): Length parameter must be greater than 0 in %s on line %d
bool(false)
string(5) "e tau"

Warning: gzread(): Length parameter must be greater than 0 in %s on line %d
bool(false)
string(8) "ght thro"
===DONE===