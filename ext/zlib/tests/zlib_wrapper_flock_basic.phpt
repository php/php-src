--TEST--
Test function flock on a zlib stream
--EXTENSIONS--
zlib
--FILE--
<?php
$f = __DIR__."/004.txt.gz";
$h = gzopen($f,'r');
var_dump(flock($h, LOCK_EX));
gzclose($h);
?>
--EXPECT--
bool(true)
