--TEST--
Check that windows version constants are initialized
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) != 'WIN') {
    die('skip.. Windows only');
}
?>
--FILE--
<?php
var_dump(PHP_WINDOWS_VERSION_MAJOR > 0, PHP_WINDOWS_VERSION_MAJOR, PHP_WINDOWS_VERSION_MINOR);
?>
==DONE==
--EXPECTF--
bool(true)
int(%d)
int(%d)
==DONE==
