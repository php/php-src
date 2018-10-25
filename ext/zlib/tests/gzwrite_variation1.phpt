--TEST--
Test function gzwrite() by calling it when file is opened for reading
--SKIPIF--
<?php
if (!extension_loaded("zlib")) {
	print "skip - ZLIB extension not loaded";
}
?>
--FILE--
<?php

$filename = dirname(__FILE__)."/004.txt.gz";
$h = gzopen($filename, 'r');
$str = "Here is the string to be written. ";
$length = 10;
var_dump(gzwrite( $h, $str ) );
var_dump(gzread($h, 10));
var_dump(gzwrite( $h, $str, $length ) );
gzclose($h);

?>
===DONE===
--EXPECT--
int(0)
string(10) "When you'r"
int(0)
===DONE===
