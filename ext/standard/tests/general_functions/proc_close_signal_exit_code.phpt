--TEST--
proc_close() returns 128+signal when child is killed by a signal
--SKIPIF--
<?php
if (PHP_OS_FAMILY === 'Windows') die('skip Not for Windows');
if (!function_exists('posix_kill') || !function_exists('posix_getpid')) die('skip posix extension required');
?>
--FILE--
<?php

// Child that kills itself with SIGTERM (15)
$child = '<?php posix_kill(posix_getpid(), 15);';

$process = proc_open(
    [PHP_BINARY, '-r', $child],
    [['pipe', 'r'], ['pipe', 'w'], ['pipe', 'w']],
    $pipes
);

foreach ($pipes as $pipe) {
    fclose($pipe);
}

$exitCode = proc_close($process);
// Should be 128 + 15 = 143
var_dump($exitCode);

// Child that exits normally with code 42
$child2 = '<?php exit(42);';

$process2 = proc_open(
    [PHP_BINARY, '-r', $child2],
    [['pipe', 'r'], ['pipe', 'w'], ['pipe', 'w']],
    $pipes2
);

foreach ($pipes2 as $pipe) {
    fclose($pipe);
}

$exitCode2 = proc_close($process2);
// Should be 42
var_dump($exitCode2);

?>
--EXPECT--
int(143)
int(42)