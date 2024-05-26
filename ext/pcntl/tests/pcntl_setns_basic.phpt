--TEST--
pcntl_setns()
--EXTENSIONS--
pcntl
posix
--SKIPIF--
<?php
if (!function_exists("pcntl_setns")) die("skip pcntl_setns is not available");
if (getenv('SKIP_ASAN')) die('skip Timeouts under ASAN');
?>
--FILE--
<?php
$pid = pcntl_fork();
if ($pid == -1) die("pcntl_fork failed");
if ($pid != 0) {
	try {
		pcntl_setns($pid, 0);
	} catch (\ValueError $e) {
		echo $e->getMessage();
	}
}
?>
--EXPECTF--
pcntl_setns(): Argument #2 ($nstype) is an invalid nstype (%d) 
