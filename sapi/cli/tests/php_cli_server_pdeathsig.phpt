--TEST--
Killing server should terminate all worker processes
--ENV--
PHP_CLI_SERVER_WORKERS=2
--SKIPIF--
<?php
include "skipif.inc";
if (!(str_contains(PHP_OS, 'Linux') || str_contains(PHP_OS, 'FreeBSD'))) {
    die('skip PDEATHSIG is only supported on Linux and FreeBSD');
}
if (@file_exists('/.dockerenv')) die("skip Broken in Docker");
?>
--FILE--
<?php

function split_words(?string $lines): array {
    return preg_split('(\s)', trim($lines ?? ''), flags: PREG_SPLIT_NO_EMPTY);
}

function find_workers_by_ppid(string $ppid) {
    return split_words(shell_exec('pgrep -P ' . $ppid));
}

function find_workers_by_pids(array $pids) {
    return split_words(shell_exec('ps -o pid= -p ' . join(',', $pids)));
}

include "php_cli_server.inc";
$cliServerInfo = php_cli_server_start('');

$master = proc_get_status($cliServerInfo->processHandle)['pid'];
$workersBefore = find_workers_by_ppid($master);
if (count($workersBefore) === 0) {
    throw new \Exception('Could not find worker pids');
}

proc_terminate($cliServerInfo->processHandle, 9); // SIGKILL

$try = 1;
$max_tries = 20;
while (true) {
    $workersAfter = find_workers_by_pids($workersBefore);
    if (count($workersAfter) === 0) {
        break;
    }
    if ($try >= $max_tries) {
        throw new \Exception('Workers were not properly terminated. Before: ' . join(', ', $workersBefore) . ', after: ' . join(', ', $workersAfter));
    }
    $try++;
    usleep(100_000);
}

echo 'Done';
?>
--EXPECT--
Done
