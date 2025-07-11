--TEST--
Test function fstat() on zlib wrapper
--EXTENSIONS--
zlib
--FILE--
<?php
$f = __DIR__."/data/test.txt.gz";
$h = gzopen($f, "r");
var_dump(fstat($h));
fclose($h);
?>
--EXPECT--
bool(false)
