--TEST--
pcntl_getcpuaffinity() and pcntl_setcpuaffinity()
--EXTENSIONS--
pcntl
--SKIPIF--
<?php
if (!function_exists("pcntl_setcpuaffinity")) die("skip pcntl_setcpuaffinity is not available");
?>
--FILE--
<?php
$mask = [0, 1];
var_dump(pcntl_setcpuaffinity(null, $mask));
$act_mask = pcntl_getcpuaffinity();
var_dump(array_diff($mask, $act_mask));
$n_act_mask = pcntl_getcpuaffinity();
var_dump(array_diff($act_mask, $n_act_mask));

try {
	pcntl_setcpuaffinity(null, []);
} catch (\ValueError $e) {
	echo $e->getMessage() . PHP_EOL;
}

try {
	pcntl_setcpuaffinity(null, ["abc" => "def", 0 => "cpuid"]);
} catch (\ValueError $e) {
	echo $e->getMessage() . PHP_EOL;
}

try {
	pcntl_setcpuaffinity(null, [PHP_INT_MAX]);
} catch (\ValueError $e) {
	echo $e->getMessage() . PHP_EOL;
}

try {
	pcntl_setcpuaffinity(null, [-1024, 64, -2]);
} catch (\ValueError $e) {
	echo $e->getMessage() . PHP_EOL;
}

try {
	pcntl_getcpuaffinity(-1024);
} catch (\ValueError $e) {
	echo $e->getMessage();
}
?>
--EXPECT--
bool(true)
array(0) {
}
array(0) {
}
pcntl_setcpuaffinity(): Argument #2 ($cpu_ids) must not be empty
pcntl_setcpuaffinity(): Argument #2 ($cpu_ids) invalid cpu affinity mapping
pcntl_setcpuaffinity(): Argument #2 ($cpu_ids) invalid cpu affinity mapping
pcntl_setcpuaffinity(): Argument #2 ($cpu_ids) invalid cpu affinity mapping
pcntl_getcpuaffinity(): Argument #1 ($process_id) invalid process (-1024)
