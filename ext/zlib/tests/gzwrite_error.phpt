--TEST--
Test function gzwrite() by calling it more than or less than its expected arguments
--SKIPIF--
<?php 
if (!extension_loaded("zlib")) {
	print "skip - ZLIB extension not loaded"; 
}
?>
--FILE--
<?php
$filename = "temp.txt.gz";
$h = gzopen($filename, 'w');
$str = "Here is the string to be written. ";
$length = 10;
$extra_arg = 'nothing'; 
var_dump(gzwrite($h, $str, $length, $extra_arg));
var_dump(gzwrite($h));
var_dump(gzwrite());

gzclose($h);
unlink($filename);

?>
===DONE===
--EXPECTF--

Warning: gzwrite() expects at most 3 parameters, 4 given in %s on line %d
bool(false)

Warning: gzwrite() expects at least 2 parameters, 1 given in %s on line %d
bool(false)

Warning: gzwrite() expects at least 2 parameters, 0 given in %s on line %d
bool(false)
===DONE===