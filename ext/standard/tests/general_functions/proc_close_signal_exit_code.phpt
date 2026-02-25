--TEST--
proc_close() returns 128+signal when child is killed by a signal
--SKIPIF--
<?php
if (PHP_OS_FAMILY === 'Windows') die('skip Not for Windows');
if (!is_executable('/bin/sh')) die('skip /bin/sh not available');
?>
--FILE--
<?php

// Shell that kills itself with SIGKILL (9) — cannot be caught
$process = proc_open(
    ['/bin/sh', '-c', 'kill -9 $$'],
    [['pipe', 'r'], ['pipe', 'w'], ['pipe', 'w']],
    $pipes
);

foreach ($pipes as $pipe) {
    fclose($pipe);
}

$exitCode = proc_close($process);
// Should be 128 + 9 = 137
var_dump($exitCode);

// Child that exits normally with code 42
$process2 = proc_open(
    ['/bin/sh', '-c', 'exit 42'],
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
int(137)
int(42)