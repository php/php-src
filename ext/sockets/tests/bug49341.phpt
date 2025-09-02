--TEST--
Bug #49341: add SO_REUSEPORT support for socket_set_option()
--EXTENSIONS--
sockets
--SKIPIF--
<?php

if (PHP_OS !== 'Darwin' && false === strpos(PHP_OS, 'BSD')) {
    die('skip is not *BSD.');
}
?>
--FILE--
<?php
var_dump(defined('SO_REUSEPORT'));
?>
--EXPECT--
bool(true)
