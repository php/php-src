--TEST--
GH-25541 (Signal whose handler was removed keeps its default action on a thread that never ran PHP)
--EXTENSIONS--
pcntl
posix
zend_test
--SKIPIF--
<?php
if (!function_exists('zend_test_start_foreign_thread')) die('skip ZTS only');
if (PHP_OS === 'Darwin') die('skip sigprocmask() blocks signals in every thread on macOS');
?>
--FILE--
<?php
$pid = pcntl_fork();
if ($pid === 0) {
    pcntl_signal(SIGUSR1, function () {});
    pcntl_signal(SIGUSR1, SIG_DFL);
    zend_test_start_foreign_thread();

    /* The PHP thread blocks the signal, so the kernel picks the other thread. */
    pcntl_sigprocmask(SIG_BLOCK, [SIGUSR1]);
    posix_kill(posix_getpid(), SIGUSR1);
    usleep(500000);
    echo "child survived", PHP_EOL;
    exit(0);
}

pcntl_waitpid($pid, $status);
var_dump(pcntl_wifsignaled($status));
var_dump(pcntl_wifsignaled($status) && pcntl_wtermsig($status) === SIGUSR1);
?>
--EXPECT--
bool(true)
bool(true)
