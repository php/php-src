--TEST--
Waiting on SIGCHLD with a pcntl_wait() loop
--EXTENSIONS--
pcntl
--SKIPIF--
<?php
if (PHP_OS != 'Linux') {
    die('skip Linux only');
}
?>
--FILE--
<?php
$processes = [];

pcntl_async_signals(true);
pcntl_signal(SIGCHLD, function($sig, $info) use (&$processes) {
    while (($pid = pcntl_wait($status, WUNTRACED | WNOHANG)) > 0) {
        echo "SIGCHLD\n";
        unset($processes[$pid]);
    }
}, false);

for ($i = 0; $i <= 5; $i++) {
    // Sleeping ensures we get to add the process to the list before the signal is invoked.
    $process = proc_open('sleep 1', [], $pipes);
    $pid = proc_get_status($process)['pid'];
    $processes[$pid] = $process;
}

$iters = 50;
while (!empty($processes) && $iters > 0) {
    usleep(100_000);
    $iters--;
}

var_dump(empty($processes));
?>
--EXPECT--
SIGCHLD
SIGCHLD
SIGCHLD
SIGCHLD
SIGCHLD
SIGCHLD
bool(true)
