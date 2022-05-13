--TEST--
pcntl: SIG_BLOCK, SIG_UNBLOCK, SIG_SETMASK
--EXTENSIONS--
pcntl
posix
--SKIPIF--
<?php
if (!function_exists('pcntl_sigwaitinfo') or !function_exists('pcntl_sigtimedwait')) die('skip required functionality is not available');
?>
--FILE--
<?php

// Clear mask
pcntl_sigprocmask(SIG_SETMASK, array(), $prev);

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

// Restore previous mask
pcntl_sigprocmask(SIG_SETMASK, $prev, $old);
var_dump(count($old));

?>
--EXPECT--
int(0)
int(2)
int(3)
int(2)
int(1)
int(0)
