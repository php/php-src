--TEST--
Calling bzerr* functions on non-bz2 streams
--SKIPIF--
<?php if (!extension_loaded("bz2")) print "skip"; ?>
--FILE--
<?php
$f = fopen(__FILE__, 'r');
var_dump(bzerrno($f));
var_dump(bzerrstr($f));
var_dump(bzerror($f));
?>
--EXPECTF--
Warning: bzerrno(): Stream is not a bz2 stream in %s on line %d
bool(false)

Warning: bzerrstr(): Stream is not a bz2 stream in %s on line %d
bool(false)

Warning: bzerror(): Stream is not a bz2 stream in %s on line %d
bool(false)
