--TEST--
GH-25541 (signal on a thread that never ran PHP, in the child of pcntl_fork())
--EXTENSIONS--
pcntl
posix
zend_test
--SKIPIF--
<?php
if (!PHP_ZTS) die('skip ZTS only');
if (!function_exists('zend_test_raise_on_foreign_thread')) die('skip zend_test_raise_on_foreign_thread() not available');
if (!zend_test_signal_forwarding('supported')) die('skip signal forwarding is not supported on this platform');
?>
--FILE--
<?php
$handled = false;
pcntl_signal(SIGUSR1, function () use (&$handled) {
    $handled = true;
});

$pid = pcntl_fork();
if ($pid === 0) {
    zend_test_raise_on_foreign_thread(SIGUSR1);
    for ($i = 0; !$handled && $i < 3000; $i++) {
        pcntl_signal_dispatch();
        usleep(10000);
    }
    echo $handled ? "child handled\n" : "child lost\n";
    /* LeakSanitizer does not support the exit of a forked child of a threaded process. */
    if (getenv('SKIP_ASAN')) {
        posix_kill(posix_getpid(), SIGKILL);
    }
    exit(0);
}

pcntl_waitpid($pid, $status);
var_dump(getenv('SKIP_ASAN') ? pcntl_wtermsig($status) === SIGKILL : pcntl_wifexited($status) && pcntl_wexitstatus($status) === 0);
?>
--EXPECT--
child handled
bool(true)
