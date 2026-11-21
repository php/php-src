--TEST--
GH-25541 (SIGABRT from another process is re-routed across several threads that never ran PHP)
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
pcntl_sigprocmask(SIG_BLOCK, [SIGUSR1]);
$pid = pcntl_fork();
if ($pid === 0) {
    $handled = false;
    pcntl_signal(SIGABRT, function () use (&$handled) {
        $handled = true;
    });
    zend_test_start_foreign_thread();
    zend_test_start_foreign_thread();

    /* The PHP thread blocks the signal: the first foreign thread re-sends it,
     * and the second one receives the re-sent signal. */
    pcntl_sigprocmask(SIG_BLOCK, [SIGABRT]);
    posix_kill(posix_getppid(), SIGUSR1);
    usleep(500000);
    pcntl_sigprocmask(SIG_UNBLOCK, [SIGABRT]);
    pcntl_signal_dispatch();
    exit($handled ? 0 : 1);
}

pcntl_sigtimedwait([SIGUSR1], $info, 5);
posix_kill($pid, SIGABRT);
pcntl_waitpid($pid, $status);
var_dump(pcntl_wifsignaled($status) ? 'signal ' . pcntl_wtermsig($status) : 'exit ' . pcntl_wexitstatus($status));
?>
--EXPECT--
string(6) "exit 0"
