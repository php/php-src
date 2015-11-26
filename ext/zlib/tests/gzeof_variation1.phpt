--TEST--
Test function gzeof while writing.
--SKIPIF--
<?php 
if (!extension_loaded("zlib")) {
	print "skip - ZLIB extension not loaded"; 
}
?>
--FILE--
<?php

$filename = dirname(__FILE__)."/gzeof_variation1.txt.gz";
$h = gzopen($filename, 'w');
$str = "Here is the string to be written. ";
$length = 10;
gzwrite( $h, $str );
var_dump(gzeof($h));
gzwrite( $h, $str, $length);
var_dump(gzeof($h));
gzclose($h);
var_dump(gzeof($h));
unlink($filename);
?>
===DONE===
--EXPECTF--
bool(false)
bool(false)

Warning: gzeof(): supplied resource is not a valid stream resource in %s on line %d
bool(false)
===DONE===
