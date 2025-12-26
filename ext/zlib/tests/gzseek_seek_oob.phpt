--TEST--
Test function gzseek() by seeking out of bounds
--EXTENSIONS--
zlib
--FILE--
<?php
$f = __DIR__."/004.txt.gz";
$h = gzopen($f, 'r');

var_dump(gzseek($h, -100, SEEK_CUR));
var_dump(gzseek($h, 0, SEEK_CUR));
var_dump(gztell($h));

gzclose($h);
?>
--EXPECTF--
int(-1)
php: %s: _php_stream_seek: Assertion `stream->position >= 0' failed.

Termsig=6
