--TEST--
ext/sockets - socket_strerror - basic test
--CREDITS--
Florian Anderiasch
fa@php.net
--EXTENSIONS--
sockets
--SKIPIF--
<?php

if (!stristr(PHP_OS, "Linux")) {
    die('skip - test validates linux error strings only.');
}
?>
--FILE--
<?php
/* Only test one representative error code here,
 * as messages will differ depending on the used libc. */
var_dump(socket_strerror(1));
?>
--EXPECT--
string(23) "Operation not permitted"
