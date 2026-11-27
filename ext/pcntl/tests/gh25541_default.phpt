--TEST--
GH-25541 (Signal without a handler, delivered to a thread that never ran PHP, keeps its default action)
--EXTENSIONS--
pcntl
zend_test
--SKIPIF--
<?php
if (!function_exists('zend_test_raise_on_foreign_thread')) die('skip ZTS only');
?>
--FILE--
<?php
pcntl_signal(SIGUSR1, function () {});

$pid = pcntl_fork();
if ($pid === 0) {
    zend_test_raise_on_foreign_thread(SIGTERM);
    usleep(500000);
    echo "child survived\n";
    exit(0);
}

pcntl_waitpid($pid, $status);
var_dump(pcntl_wifsignaled($status));
var_dump(pcntl_wtermsig($status) === SIGTERM);
?>
--EXPECT--
bool(true)
bool(true)
