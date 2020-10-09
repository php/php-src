--TEST--
pcntl_unshare() with CLONE_NEWNET
--SKIPIF--
<?php
if (!extension_loaded("pcntl")) die("skip");
if (!extension_loaded("posix")) die("skip posix extension not available");
if (!function_exists("pcntl_unshare")) die("skip pcntl_unshare is not available");
if (!defined("CLONE_NEWNET")) die("skip flag unavailable");
if (posix_getuid() !== 0 &&
    (!defined("CLONE_NEWUSER") ||
    (pcntl_unshare(CLONE_NEWUSER) == false && pcntl_get_last_error() == PCNTL_EPERM))) {
    die("skip Insufficient privileges for CLONE_NEWUSER");
}
if (@pcntl_unshare(CLONE_NEWNET) == false && pcntl_get_last_error() == PCNTL_EPERM) {
    die("skip Insufficient privileges for CLONE_NEWPID");
}
if (getenv("SKIP_ONLINE_TESTS")) die("skip online test");
?>
--FILE--
<?php

if(posix_getuid() !== 0) {
    pcntl_unshare(CLONE_NEWUSER);
}

var_dump(gethostbyname('php.net'));
pcntl_unshare(CLONE_NEWNET);
var_dump(gethostbyname('php.net'));
?>
--EXPECTF--
string(%d) %s
string(7) "php.net"
