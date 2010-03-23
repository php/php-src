--TEST--
Test posix_mkfifo() with safe_mode.
--DESCRIPTION--
The test attempts to enable safe_mode, catches all the relevant E_WARNING's and tries to create a fifo in /tmp.

The first attempt (writing to /tmp) should effectively fail because /tmp is owned by root.

The second attempt (writing to a local created file) works.
--CREDITS--
Till Klampaeckel, till@php.net
TestFest Berlin 2009
--SKIPIF--
<?php
if (!extension_loaded('posix')) {
    die('SKIP The posix extension is not loaded.');
}
if (posix_geteuid() == 0) {
    die('SKIP Cannot run test as root.');
}
?>
--INI--
safe_mode = 1
--FILE--
<?php
var_dump(posix_mkfifo('/tmp/foobar', 0644));

$dir = dirname(__FILE__) . '/foo';
mkdir ($dir);
var_dump(posix_mkfifo($dir . '/bar', 0644));
?>
===DONE===
--CLEAN--
<?php
$dir = dirname(__FILE__) . '/foo';
unlink($dir . '/bar');
rmdir($dir);
?>
--EXPECTF--
Warning: Directive 'safe_mode' is deprecated in PHP 5.3 and greater in %s on line %d

Warning: posix_mkfifo(): SAFE MODE Restriction in effect.  The script whose uid is %d is not allowed to access /tmp owned by uid %d in %s on line %d
bool(false)
bool(true)
===DONE===
