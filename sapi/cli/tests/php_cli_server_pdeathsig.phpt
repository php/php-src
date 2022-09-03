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
?>
--FILE--
<?php

function split_ps_pids(?string $lines): array {
    return preg_split('(\R)', trim($lines ?? ''), flags: PREG_SPLIT_NO_EMPTY);
}

function get_worker_pids_by_ppid(string $ppid): array {
    return split_ps_pids(shell_exec('ps -o pid= --ppid ' . $ppid));
}

function get_worker_pids_by_pids(array $pids): array {
    $pidFilters = array_map(fn (string $pid) => '--pid ' . $pid, $pids);
    return split_ps_pids(shell_exec('ps -o pid= ' . join(' ', $pidFilters)));
}

include "php_cli_server.inc";
$cliServerInfo = php_cli_server_start('');

$ppid = proc_get_status($cliServerInfo->processHandle)['pid'];

$workerPids = get_worker_pids_by_ppid($ppid);
if (count($workerPids) === 0) {
    throw new \Exception('Could not find worker pids');
}

proc_terminate($cliServerInfo->processHandle, 9); // SIGKILL
$workerPids = get_worker_pids_by_pids($workerPids);

if (count($workerPids) !== 0) {
    throw new \Exception('Workers were not properly terminated');
}

echo 'Done';
?>
--EXPECT--
Done
