--TEST--
Test function gzread() by calling it while file open for writing
--SKIPIF--
<?php 
if (!extension_loaded("zlib")) {
	print "skip - ZLIB extension not loaded"; 
}
?>
--FILE--
<?php

$filename = "gzread_variation1.txt.gz";
$h = gzopen($filename, 'w');
$str = "Here is the string to be written. ";
var_dump(gzread($h, 100));
gzwrite( $h, $str);
var_dump(gzread($h, 100));
gzrewind($h);
var_dump(gzread($h, 100));
gzclose($h);

$h = gzopen($filename, 'r');
gzpassthru($h);
gzclose($h);
echo "\n";
unlink($filename);
?>
===DONE===
--EXPECT--
string(0) ""
string(0) ""
string(0) ""
Here is the string to be written. 
===DONE===