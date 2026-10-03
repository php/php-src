--TEST--
pcntl_sigtimedwait(): a timeout too large for nanoseconds waits instead of wrapping around
--EXTENSIONS--
pcntl
--SKIPIF--
<?php
if (!function_exists('pcntl_sigtimedwait')) die('skip required functionality is not available');
if (PHP_INT_SIZE < 8) die('skip 64-bit only');
?>
--FILE--
<?php
pcntl_sigprocmask(SIG_BLOCK, [SIGALRM]);
pcntl_alarm(1);
// 18446744074 seconds in nanoseconds wraps to about 0.3 seconds
var_dump(pcntl_sigtimedwait([SIGALRM], $info, 18446744074) === SIGALRM);
?>
--EXPECT--
bool(true)
