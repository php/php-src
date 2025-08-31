--TEST--
pcntl_unshare() with CLONE_NEWPID
--EXTENSIONS--
pcntl
posix
--SKIPIF--
<?php
if (!function_exists("pcntl_unshare")) die("skip pcntl_unshare is not available");
if (!defined("CLONE_NEWPID")) die("skip flag unavailable");
if (getenv("SKIP_ASAN")) die("skip asan chokes on this");
if (posix_getuid() !== 0 &&
    (!defined("CLONE_NEWUSER") ||
    (@pcntl_unshare(CLONE_NEWUSER) == false && pcntl_get_last_error() == PCNTL_EPERM))) {
    die("skip Insufficient privileges for CLONE_NEWUSER");
}
if (@pcntl_unshare(CLONE_NEWPID) == false && pcntl_get_last_error() == PCNTL_EPERM) {
    die("skip Insufficient privileges for CLONE_NEWPID");
}
if (getenv("SKIP_REPEAT")) die("skip cannot be repeated");
?>
--FILE--
<?php

if(posix_getuid() !== 0) {
    pcntl_unshare(CLONE_NEWUSER);
}

var_dump(getmypid());
pcntl_unshare(CLONE_NEWPID);
if(!pcntl_fork()) {
    var_dump(getmypid());
    exit();
}
?>
--EXPECTF--
int(%d)
int(1)
