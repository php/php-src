--TEST--
pcntl_setcpuaffinity(): the upper bound the error advertises is itself a valid cpu id
--EXTENSIONS--
pcntl
--SKIPIF--
<?php
if (PHP_OS_FAMILY === 'Solaris') {
    die("skip broken pset_create()");
}
if (!function_exists("pcntl_setcpuaffinity")) die("skip pcntl_setcpuaffinity is not available");
?>
--FILE--
<?php
$pid = getmypid();
$prefix = 'pcntl_setcpuaffinity(): Argument #2 ($cpu_ids) cpu id must be between 0 and ';

/* read the advertised bound out of the message itself */
try {
    pcntl_setcpuaffinity($pid, [PHP_INT_MAX]);
    exit("PHP_INT_MAX was accepted as a cpu id" . PHP_EOL);
} catch (ValueError $e) {
    if (!preg_match('/must be between 0 and (\d+) \(/', $e->getMessage(), $m)) {
        exit("unexpected message: " . $e->getMessage() . PHP_EOL);
    }
}
$bound = (int) $m[1];

/* Every id is range checked before any syscall runs, so pairing the advertised
   bound with an out of range id shows which of the two the check rejects,
   without ever changing the process affinity. */
try {
    pcntl_setcpuaffinity($pid, [$bound, PHP_INT_MAX]);
} catch (ValueError $e) {
    var_dump($e->getMessage() === $prefix . $bound . ' (' . PHP_INT_MAX . ')');
}

/* and the first id past the bound is rejected, naming itself */
try {
    pcntl_setcpuaffinity($pid, [$bound + 1]);
} catch (ValueError $e) {
    var_dump($e->getMessage() === $prefix . $bound . ' (' . ($bound + 1) . ')');
}
?>
--EXPECT--
bool(true)
bool(true)
