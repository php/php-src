--TEST--
Test parameter validation in posix_mkfifo().
--CREDITS--
Till Klampaeckel, till@php.net
TestFest Berlin 2009
--SKIPIF--
<?php
if (!extension_loaded('posix')) {
    die('SKIP The posix extension is not loaded.');
}
?>
--FILE--
<?php
posix_mkfifo(null);
var_dump(posix_mkfifo(null, 0644));
?>
===DONE===
--EXPECTF--
Warning: posix_mkfifo() expects exactly 2 parameters, 1 given in %s on line %d
bool(false)
===DONE===
