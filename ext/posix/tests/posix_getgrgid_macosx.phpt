--TEST--
Test return values of posix_getgrgid() on MacOSX.
--CREDITS--
Till Klampaeckel, till@php.net
TestFest Berlin 2009
--EXTENSIONS--
posix
--SKIPIF--
<?php

if (strtolower(PHP_OS) != 'darwin') {
    die('SKIP This test requires MacOSX/Darwin.');
}
?>
--FILE--
<?php
$grp = posix_getgrgid(-1);
var_dump($grp['name']);
?>
--EXPECT--
string(7) "nogroup"
