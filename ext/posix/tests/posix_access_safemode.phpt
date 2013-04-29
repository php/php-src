--TEST--
Test posix_access() with safe_mode enabled.
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
if (PHP_VERSION_ID < 503099) {
    die('SKIP Safe mode is no longer available.');
}
--FILE--
<?php
var_dump(posix_access('/tmp', POSIX_W_OK));
?>
===DONE===
--EXPECTF--
Deprecated: Directive 'safe_mode' is deprecated in PHP 5.3 and greater in %s on line %d
bool(false)
===DONE===
