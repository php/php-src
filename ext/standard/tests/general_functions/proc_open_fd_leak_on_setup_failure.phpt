--TEST--
proc_open() does not leak file descriptors when descriptor setup fails mid-spec
--SKIPIF--
<?php
if (!function_exists("proc_open")) die("skip proc_open() unavailable");
if (!@is_dir("/proc/self/fd")) die("skip requires /proc/self/fd");
?>
--FILE--
<?php
$before = count(scandir("/proc/self/fd"));
for ($i = 0; $i < 100; $i++) {
    // Index 0 opens a real pipe; index 1 is invalid, so setup fails after the
    // pipe is already open. The aborted call must release the pipe fds.
    @proc_open("true", [0 => ["pipe", "r"], 1 => ["bogus_type"]], $pipes);
}
$after = count(scandir("/proc/self/fd"));
var_dump($after <= $before + 2);
?>
--EXPECT--
bool(true)
