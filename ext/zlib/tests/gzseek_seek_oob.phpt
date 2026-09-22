--TEST--
Test function gzseek() by seeking out of bounds
--EXTENSIONS--
zlib
--FILE--
<?php
$f = __DIR__."/data/test.txt.gz";
$h = gzopen($f, 'r');

var_dump(gzseek($h, -100, SEEK_CUR));
var_dump(gzseek($h, 0, SEEK_CUR));
var_dump(gztell($h));

gzclose($h);
?>
--EXPECT--
int(-1)
int(0)
int(0)
