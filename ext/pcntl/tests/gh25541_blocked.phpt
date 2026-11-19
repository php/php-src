--TEST--
GH-25541 (Signal re-routed while the PHP thread blocks it is handled once unblocked)
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
pcntl_signal(SIGUSR1, function () use (&$handled) {
    $handled = true;
});
zend_test_start_foreign_thread();

/* The PHP thread blocks the signal, so the kernel picks the other thread. */
pcntl_sigprocmask(SIG_BLOCK, [SIGUSR1]);
posix_kill(posix_getpid(), SIGUSR1);
usleep(100000);
pcntl_sigprocmask(SIG_UNBLOCK, [SIGUSR1]);
pcntl_signal_dispatch();
var_dump($handled);
?>
--EXPECT--
bool(true)
