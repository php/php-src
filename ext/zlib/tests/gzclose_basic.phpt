--TEST--
Test function gzclose() by calling it with its expected arguments
--SKIPIF--
<?php
if (!extension_loaded("zlib")) {
	print "skip - ZLIB extension not loaded";
}
?>
--FILE--
<?php
// note that gzclose is an alias to fclose. parameter checking tests will be
// the same as fclose

$f = __DIR__."/004.txt.gz";
$h = gzopen($f, 'r');
gzread($h, 20);
var_dump(gzclose($h));

//should fail.
gzread($h, 20);

$h = gzopen($f, 'r');
gzread($h, 20);
var_dump(fclose($h));

//should fail.
gzread($h, 20);


?>
===DONE===
--EXPECTF--
bool(true)

Warning: gzread(): supplied resource is not a valid stream resource in %s on line %d
bool(true)

Warning: gzread(): supplied resource is not a valid stream resource in %s on line %d
===DONE===
