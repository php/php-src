--TEST--
GH-25541 (signal on a thread that never ran PHP, in the child of pcntl_fork())
--EXTENSIONS--
pcntl
zend_test
--SKIPIF--
<?php
if (!PHP_ZTS) die('skip ZTS only');
if (!function_exists('zend_test_raise_on_foreign_thread')) die('skip zend_test_raise_on_foreign_thread() not available');
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
    for ($i = 0; !$handled && $i < 100; $i++) {
        pcntl_signal_dispatch();
        usleep(10000);
    }
    echo $handled ? "child handled\n" : "child lost\n";
    exit(0);
}

pcntl_waitpid($pid, $status);
var_dump(pcntl_wifexited($status));
?>
--EXPECT--
child handled
bool(true)
