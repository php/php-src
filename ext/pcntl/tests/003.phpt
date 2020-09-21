--TEST--
pcntl: SIG_BLOCK, SIG_UNBLOCK, SIG_SETMASK
--SKIPIF--
<?php
    if (!extension_loaded('pcntl')) die('skip pcntl extension not available');
    elseif (!extension_loaded('posix')) die('skip posix extension not available');
    elseif (!function_exists('pcntl_sigwaitinfo') or !function_exists('pcntl_sigtimedwait')) die('skip required functionality is not available');
?>
--FILE--
<?php

pcntl_sigprocmask(SIG_BLOCK, array(SIGCHLD,SIGTERM), $old);
var_dump(count($old));
pcntl_sigprocmask(SIG_BLOCK, array(SIGINT), $old);
var_dump(count($old));
pcntl_sigprocmask(SIG_UNBLOCK, array(SIGINT), $old);
var_dump(count($old));
pcntl_sigprocmask(SIG_SETMASK, array(SIGINT), $old);
var_dump(count($old));
pcntl_sigprocmask(SIG_SETMASK, array(), $old);
var_dump(count($old));
pcntl_sigprocmask(SIG_SETMASK, array(), $old);
var_dump(count($old));

?>
--EXPECT--
int(0)
int(2)
int(3)
int(2)
int(1)
int(0)
