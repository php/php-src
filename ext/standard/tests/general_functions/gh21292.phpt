--TEST--
GH-21292 (proc_close() returns raw signal number, indistinguishable from normal exit code)
--SKIPIF--
<?php
if (PHP_OS_FAMILY === 'Windows') die('skip Not for Windows');
if (!is_executable('/bin/sh')) die('skip /bin/sh not available');
?>
--FILE--
<?php

function spawn(string $script) {
    $process = proc_open(
        ['/bin/sh', '-c', $script],
        [['pipe', 'r'], ['pipe', 'w'], ['pipe', 'w']],
        $pipes
    );
    foreach ($pipes as $pipe) {
        fclose($pipe);
    }
    return $process;
}

echo "Killed by SIGKILL:\n";
var_dump(proc_close(spawn('kill -9 $$')));

echo "Normal exit:\n";
var_dump(proc_close(spawn('exit 42')));
var_dump(proc_close(spawn('exit 9')));
var_dump(proc_close(spawn('exit 137')));

echo "Killed by SIGKILL, status polled before proc_close():\n";
$process = spawn('kill -9 $$');
for ($i = 0; $i < 500 && proc_get_status($process)['running']; $i++) {
    usleep(10000);
}
$status = proc_get_status($process);
var_dump($status['signaled'], $status['termsig']);
$status = proc_get_status($process);
var_dump($status['cached'], $status['signaled'], $status['termsig']);
var_dump(proc_close($process));

?>
--EXPECT--
Killed by SIGKILL:
int(137)
Normal exit:
int(42)
int(9)
int(137)
Killed by SIGKILL, status polled before proc_close():
bool(true)
int(9)
bool(true)
bool(true)
int(9)
int(137)
