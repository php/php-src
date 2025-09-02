--TEST--
Test function flock on a zlib stream
--EXTENSIONS--
zlib
--FILE--
<?php
$f = __DIR__."/data/test.txt.gz";
$h = gzopen($f,'r');
var_dump(flock($h, LOCK_EX));
gzclose($h);
?>
--EXPECT--
bool(true)
