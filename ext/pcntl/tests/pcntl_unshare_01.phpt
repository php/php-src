--TEST--
pcntl_unshare() with CLONE_NEWUSER
--SKIPIF--
<?php
if (!extension_loaded("pcntl")) die("skip");
if (!extension_loaded("posix")) die("skip posix extension not available");
if (!function_exists("pcntl_unshare")) die("skip pcntl_unshare is not available");
if (!defined("CLONE_NEWUSER")) die("skip flag unavailable");
if (@pcntl_unshare(CLONE_NEWUSER) == false && pcntl_get_last_error() == PCNTL_EPERM) {
    die("skip Insufficient privileges to use CLONE_NEWUSER");
}
?>
--FILE--
<?php

$olduid = posix_getuid();
pcntl_unshare(CLONE_NEWUSER);
$newuid = posix_getuid();
var_dump($olduid === $newuid);
?>
--EXPECT--
bool(false)
