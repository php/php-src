--TEST--
Test function fstat() on zlib wrapper
--SKIPIF--
<?php
if (!extension_loaded("zlib")) {
	print "skip - ZLIB extension not loaded";
}
?>
--FILE--
<?php
$f = __DIR__."/004.txt.gz";
$h = gzopen($f, "r");
var_dump(fstat($h));
fclose($h);
?>
===DONE===
--EXPECT--
bool(false)
===DONE===
