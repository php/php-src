--TEST--
GH-25541 (forwarding keeps working in the parent of pcntl_rfork())
--EXTENSIONS--
pcntl
posix
zend_test
--SKIPIF--
<?php
if (!PHP_ZTS) die('skip ZTS only');
if (!function_exists('pcntl_rfork')) die('skip pcntl_rfork() is not available');
if (!function_exists('zend_test_raise_on_foreign_thread')) die('skip zend_test_raise_on_foreign_thread() not available');
if (!zend_test_signal_forwarding('supported')) die('skip signal forwarding is not supported on this platform');
?>
--FILE--
<?php
$handled = false;
pcntl_signal(SIGUSR1, function () use (&$handled) {
    $handled = true;
});

/* The child shares the file descriptor table of its parent. */
$pid = pcntl_rfork(RFPROC);
if ($pid === 0) {
    /* LeakSanitizer does not support the exit of a forked child of a threaded process. */
    if (getenv('SKIP_ASAN')) {
        posix_kill(posix_getpid(), SIGKILL);
    }
    exit(0);
}
pcntl_waitpid($pid, $status);

zend_test_raise_on_foreign_thread(SIGUSR1);
for ($i = 0; !$handled && $i < 3000; $i++) {
    pcntl_signal_dispatch();
    usleep(10000);
}
var_dump($handled);
?>
--EXPECT--
bool(true)
