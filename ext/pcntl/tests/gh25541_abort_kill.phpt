--TEST--
GH-25541 (SIGABRT sent with kill() by the process itself is re-routed from a thread that never ran PHP)
--EXTENSIONS--
pcntl
posix
zend_test
--SKIPIF--
<?php
if (!function_exists('zend_test_start_foreign_thread')) die('skip ZTS only');
if (!in_array(PHP_OS, ['Linux', 'FreeBSD'], true)) die('skip needs per-thread sigprocmask() and re-routing');
?>
--FILE--
<?php
$handled = false;
pcntl_signal(SIGABRT, function () use (&$handled) {
    $handled = true;
});
zend_test_start_foreign_thread();

/* The PHP thread blocks the signal, so the kernel picks the other thread. */
pcntl_sigprocmask(SIG_BLOCK, [SIGABRT]);
posix_kill(posix_getpid(), SIGABRT);
usleep(100000);
pcntl_sigprocmask(SIG_UNBLOCK, [SIGABRT]);
for ($wait = 0; !$handled && $wait < 500; $wait++) {
    pcntl_signal_dispatch();
    usleep(10000);
}
var_dump($handled);
?>
--EXPECT--
bool(true)
