--TEST--
pcntl_setns()
--EXTENSIONS--
pcntl
posix
--SKIPIF--
<?php
if (!function_exists("pcntl_setns")) die("skip pcntl_setns is not available");
if (posix_getuid() !== 0) die('skip Test needs root user');
if (getenv('SKIP_ASAN')) die('skip Timeouts under ASAN');
?>
--FILE--
<?php
$pid = pcntl_fork();
if ($pid == -1) die("pcntl_fork failed");
if ($pid != 0) {
	var_dump(pcntl_setns($pid, CLONE_NEWPID));
}
?>
--EXPECT--
bool(true)
