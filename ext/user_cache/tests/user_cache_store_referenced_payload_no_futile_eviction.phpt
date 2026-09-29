--TEST--
UserCache\Cache: a value that cannot fit beside the payloads still referenced by readers fails without evicting or clearing entries
--SKIPIF--
<?php
if (!function_exists('pcntl_fork')) die('skip pcntl not available');
?>
--INI--
user_cache.enable=1
user_cache.enable_cli=1
user_cache.shm_size=4M
user_cache.eviction_policy=lru
--FILE--
<?php
use UserCache\Cache;

Cache::getStatus();

foreach (['kept', 'overwritten', 'deleted'] as $mode) {
    $pid = pcntl_fork();
    if ($pid === 0) {
        $cache = Cache::getPool('store-referenced-payload-no-futile-eviction');
        $cache->clear();
        $cache->store('big', range(1, 40000));
        $held = $cache->fetch('big');
        if ($mode === 'overwritten') {
            $cache->store('big', 1);
        } elseif ($mode === 'deleted') {
            $cache->delete('big');
        }
        for ($i = 0; $i < 50; $i++) {
            $cache->store("small$i", $i);
        }

        $before = Cache::getStatus();
        $stored = $cache->store('huge', str_repeat('h', 3500000));
        $after = Cache::getStatus();

        echo $mode, ': ', json_encode([
            'stored' => $stored,
            'entries kept' => $after->getEntryCount() === $before->getEntryCount(),
            'nothing evicted' => $after->getEvictionCount() === $before->getEvictionCount(),
            'not cleared' => $after->getExpungeCount() === $before->getExpungeCount(),
            'small49' => $cache->fetch('small49'),
            'held' => count($held),
        ]), "\n";
        exit(0);
    }
    pcntl_waitpid($pid, $status);
}
?>
--EXPECT--
kept: {"stored":false,"entries kept":true,"nothing evicted":true,"not cleared":true,"small49":49,"held":40000}
overwritten: {"stored":false,"entries kept":true,"nothing evicted":true,"not cleared":true,"small49":49,"held":40000}
deleted: {"stored":false,"entries kept":true,"nothing evicted":true,"not cleared":true,"small49":49,"held":40000}
