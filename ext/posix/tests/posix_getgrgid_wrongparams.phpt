--TEST--
Test parameters on posix_getgrgid().
--CREDITS--
Till Klampaeckel, till@php.net
TestFest Berlin 2009
--SKIPIF--
<?php
if (!extension_loaded('posix')) {
    die('SKIP The posix extension is not loaded.');
}
if (strtolower(PHP_OS) == 'darwin') {
    die('SKIP This test doesn\'t run on MacOSX/Darwin.');
}
--FILE--
<?php
$gid = PHP_INT_MAX; // obscene high gid
var_dump(posix_getgrgid($gid));
var_dump(posix_getgrgid(-1));
var_dump(posix_getgrgid());
?>
===DONE===
--EXPECTF--
bool(false)
bool(false)

Warning: posix_getgrgid() expects exactly 1 parameter, 0 given in %s on line %d
bool(false)
===DONE===
