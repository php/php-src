--TEST--
GH-25541 (abort() on a thread that never ran PHP keeps its default action although SIGABRT is handled)
--EXTENSIONS--
pcntl
zend_test
--SKIPIF--
<?php
if (!function_exists('zend_test_abort_on_foreign_thread')) die('skip ZTS only');
?>
--FILE--
<?php
$pid = pcntl_fork();
if ($pid === 0) {
    pcntl_signal(SIGABRT, function () {});
    zend_test_abort_on_foreign_thread();
    usleep(500000);
    echo "child survived\n";
    exit(0);
}

pcntl_waitpid($pid, $status);
var_dump(pcntl_wifsignaled($status));
var_dump(pcntl_wifsignaled($status) && pcntl_wtermsig($status) === SIGABRT);
?>
--EXPECT--
bool(true)
bool(true)
