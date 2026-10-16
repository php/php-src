--TEST--
UserCache\Cache: user_cache.eviction_policy selects lru, clear or none behavior
--SKIPIF--
<?php
if (!function_exists('proc_open')) die('skip proc_open() not available');
?>
--INI--
user_cache.enable=1
user_cache.enable_cli=1
user_cache.shm_size=4M
--FILE--
<?php
$php = getenv('TEST_PHP_EXECUTABLE') ?: PHP_BINARY;
$args = ['-n', '-d', 'user_cache.enable=1', '-d', 'user_cache.enable_cli=1', '-d', 'user_cache.shm_size=4M'];
$code = <<<'CODE'
$cache = UserCache\Cache::getPool('p');
$blob = str_repeat('x', 8192);
$ok = 0;
for ($i = 0; $i < 600; $i++) {
    if ($cache->store('k' . $i, $blob . $i)) {
        $ok++;
    }
}
/* Every entry that survived eviction or wiping still holds the value stored under its key. */
$keys = $cache->getPoolStatus()->getEntryKeys();
$intact = $keys !== [];
foreach ($keys as $key) {
    if ($cache->fetch($key) !== $blob . substr($key, 1)) {
        $intact = false;
    }
}
$status = UserCache\Cache::getStatus();
printf(
    "stored=%d full=%d evicted=%d wiped=%d failed=%d intact=%d\n",
    $ok,
    (int) ($status->getEntryCount() > 400),
    (int) ($status->getEvictionCount() > 0),
    (int) ($status->getExpungeCount() > 0),
    (int) ($status->getStoreFailureCount() > 0),
    (int) $intact
);
CODE;

foreach (['lru', 'clear', 'none'] as $policy) {
    $process = proc_open([$php, ...$args, '-d', "user_cache.eviction_policy=$policy", '-r', $code], [1 => ['pipe', 'w']], $pipes);
    echo $policy, ': ', stream_get_contents($pipes[1]);
    fclose($pipes[1]);
    proc_close($process);
}
?>
--EXPECTF--
lru: stored=600 full=1 evicted=1 wiped=0 failed=0 intact=1
clear: stored=600 full=0 evicted=0 wiped=1 failed=0 intact=1
none: stored=%d full=1 evicted=0 wiped=0 failed=1 intact=1
