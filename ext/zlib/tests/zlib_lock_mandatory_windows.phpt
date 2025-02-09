--TEST--
Test mandatory locking on Windows
--EXTENSIONS--
zlib
--SKIPIF--
<?php
if (PHP_OS_FAMILY !== "Windows") die("skip Only for Windows because it has mandatory locking");
?>
--FILE--
<?php

echo "without wrapper\n";
$f = __DIR__."/004.txt.gz";
$h = gzopen($f,'r');
$h2 = gzopen($f, 'r');
stream_set_chunk_size($h2,1);

var_dump(flock($h, LOCK_EX));
var_dump(fread($h2, 1));
var_dump(fread($h, 1));

gzclose($h);
gzclose($h2);

echo "\nwith wrapper\n";
$f = "compress.zlib://".__DIR__."/004.txt.gz";
$h = fopen($f,'r');
$h2 = fopen($f, 'r');
stream_set_chunk_size($h2, 1);

var_dump(flock($h, LOCK_EX));
var_dump(fread($h2, 1));
var_dump(fread($h, 1));

gzclose($h);
gzclose($h2);

?>
--EXPECTF--
without wrapper
bool(true)

Notice: fread(): Read of 1 bytes failed with errno=13 Permission denied in %s on line %d
bool(false)
string(1) "W"

with wrapper
bool(true)

Notice: fread(): Read of 1 bytes failed with errno=13 Permission denied in %s on line %d
bool(false)
string(1) "W"
