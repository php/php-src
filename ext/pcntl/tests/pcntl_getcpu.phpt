--TEST--
pcntl_getcpu()
--EXTENSIONS--
pcntl
--SKIPIF--
<?php
if (!function_exists("pcntl_getcpu")) die("skip pcntl_getcpu() is not available");
if (!function_exists("pcntl_setcpuaffinity")) die("skip pcntl_setcpuaffinity() is not available");
if (getenv('SKIP_REPEAT')) die("skip Not repeatable");
?>
--FILE--
<?php
$pid = pcntl_fork();
if ($pid == -1) {
	die("fork failed");
} else if ($pid == 0) {
	var_dump(pcntl_setcpuaffinity(null, [1]));
	var_dump(pcntl_getcpu());
}
?>
--EXPECTF--
bool(true)
int(1)
