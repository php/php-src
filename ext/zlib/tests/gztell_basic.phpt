--TEST--
Test function gztell() by calling it with its expected arguments when reading
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
$intervals = array(7, 22, 54, 17, 27, 15, 1000);
// tell should be 7, 29, 83, 100, 127, 142, 176 (176 is length of uncompressed file)

var_dump(gztell($h));
foreach ($intervals as $interval) {
   gzread($h, $interval);
   var_dump(gztell($h));
}

gzclose($h);
?>
===DONE===
--EXPECT--
int(0)
int(7)
int(29)
int(83)
int(100)
int(127)
int(142)
int(176)
===DONE===