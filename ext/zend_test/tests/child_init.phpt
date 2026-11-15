--TEST--
GH-22616: Extension child initialization runs once per fork in the child
--EXTENSIONS--
zend_test
pcntl
--FILE--
<?php
var_dump(zend_test_child_init_count());
$pid = pcntl_fork();
if ($pid === -1) {
    throw new Exception('fork failed');
}
if ($pid === 0) {
    var_dump(zend_test_child_init_count());
    $pid = pcntl_fork();
    if ($pid === -1) {
        throw new Exception('fork failed');
    }
    if ($pid === 0) {
        var_dump(zend_test_child_init_count());
        exit;
    }
    pcntl_waitpid($pid, $status);
    var_dump(pcntl_wifexited($status) && pcntl_wexitstatus($status) === 0);
    var_dump(zend_test_child_init_count());
    exit;
}
pcntl_waitpid($pid, $status);
var_dump(pcntl_wifexited($status) && pcntl_wexitstatus($status) === 0);
var_dump(zend_test_child_init_count());
?>
--EXPECT--
int(0)
int(1)
int(2)
bool(true)
int(1)
bool(true)
int(0)
