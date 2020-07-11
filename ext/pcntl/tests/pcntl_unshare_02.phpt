--TEST--
pcntl_unshare() with CLONE_NEWPID
--SKIPIF--
<?php 
if (!extension_loaded("pcntl")) die("skip");
if (!extension_loaded("posix")) die("skip posix extension not available");
if (!function_exists("pcntl_unshare")) die("skip pcntl_unshare is not available");
if (!defined("CLONE_NEWPID")) die("skip flag unavailable");
if (getenv("SKIP_ASAN")) die("skip asan chokes on this");
if (posix_getuid() !== 0 &&
    (!defined("CLONE_NEWUSER") ||
    (pcntl_unshare(CLONE_NEWUSER) == false && pcntl_get_last_error() == PCNTL_EPERM))) {
    die("skip Insufficient previleges to run test");
}

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
--EXPECTF--
int(%d)
int(1)
