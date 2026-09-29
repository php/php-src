--TEST--
UserCache\Cache: a process-private segment is formatted without touching its zero-filled entry table, so the table pages are faulted in only as entries use them
--SKIPIF--
<?php
if (PHP_OS_FAMILY !== 'Linux') die('skip needs the RssShmem line of /proc/self/status');
if (!preg_match('/^RssShmem:/m', (string) @file_get_contents('/proc/self/status'))) die('skip kernel does not report RssShmem');
if (!function_exists('proc_open')) die('skip proc_open() not available');
?>
--FILE--
<?php

$code = <<<'PHP'
function rss_shmem_kb(): int
{
    preg_match('/^RssShmem:\s+(\d+) kB/m', file_get_contents('/proc/self/status'), $m);

    return (int) $m[1];
}

$before = rss_shmem_kb();
$cache = UserCache\Cache::getPool('lazy-table');
$stored = $cache->store('k', 'v') && $cache->fetch('k') === 'v';
$touchedKb = rss_shmem_kb() - $before;
$status = UserCache\Cache::getStatus();
$tableKb = intdiv($status->getEntryCapacity() * 44, 1024);

echo $status->getAvailability()->name, ' ', var_export($stored, true), ' ', var_export($tableKb > 32 * 1024 && $touchedKb < intdiv($tableKb, 8), true);
PHP;

foreach (['mmap', 'posix', 'shm'] as $model) {
    $process = proc_open(
        [
            PHP_BINARY, '-n',
            '-d', 'user_cache.enable_cli=1',
            '-d', 'user_cache.shm_size=256M',
            '-d', 'user_cache.entries_hint=1000000',
            '-d', 'user_cache.preferred_memory_model=' . $model,
            '-r', $code,
        ],
        [['pipe', 'r'], ['pipe', 'w'], ['pipe', 'w']],
        $pipes
    );
    fclose($pipes[0]);
    $out = stream_get_contents($pipes[1]) . stream_get_contents($pipes[2]);
    fclose($pipes[1]);
    fclose($pipes[2]);
    proc_close($process);

    echo $model, ': ', trim($out), "\n";
}
?>
--EXPECT--
mmap: Available true true
posix: Available true true
shm: Available true true
