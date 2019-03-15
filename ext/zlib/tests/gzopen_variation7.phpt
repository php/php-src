--TEST--
Test function gzopen() by calling it twice on the same file and not closing one of them at the end of the script
--SKIPIF--
<?php
if (!extension_loaded("zlib")) {
	print "skip - ZLIB extension not loaded";
}
?>
--FILE--
<?php

$f = __DIR__."/004.txt.gz";
$h1 = gzopen($f, 'r');
$h2 = gzopen($f, 'r');

var_dump(gzread($h1, 30));
var_dump(gzread($h2, 10));
var_dump(gzread($h1, 15));
gzclose($h1);
var_dump(gzread($h2, 50));
// deliberately do not close $h2
?>
===DONE===
--EXPECT--
string(30) "When you're taught through fee"
string(10) "When you'r"
string(15) "lings
Destiny f"
string(50) "e taught through feelings
Destiny flying high abov"
===DONE===
