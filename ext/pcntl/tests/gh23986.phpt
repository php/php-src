--TEST--
GH-23986 (/proc/self paths resolve to the parent process after pcntl_fork())
--EXTENSIONS--
pcntl
--SKIPIF--
<?php
if (!is_readable('/proc/self/stat')) die('skip /proc/self/stat not available');
?>
--FILE--
<?php
function proc_self_pid() {
    return (int) explode(' ', file_get_contents('/proc/self/stat'))[0];
}

var_dump(proc_self_pid() === getmypid());
$parent_inode = fileinode('/proc/self');

$pid = pcntl_fork();
if ($pid === 0) {
    var_dump(fileinode('/proc/self') !== $parent_inode);
    var_dump(proc_self_pid() === getmypid());
    exit(0);
}
pcntl_waitpid($pid, $status);
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
