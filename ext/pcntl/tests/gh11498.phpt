--TEST--
GH-11498 (SIGCHLD is not always returned from proc_open)
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
    unset($processes[$info['pid']]);
}, false);

foreach (range(0, 5) as $i) {
    $process = proc_open('echo $$ > /dev/null', [], $pipes);
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
bool(true)
