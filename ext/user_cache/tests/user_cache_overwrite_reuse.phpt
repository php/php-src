--TEST--
UserCache\Cache: repeated stores reuse existing entries
--INI--
user_cache.enable=1
user_cache.enable_cli=1
opcache.file_cache_only=0
user_cache.shm_size=16M
--FILE--
<?php
$cache = UserCache\Cache::getPool('overwrite-reuse');

for ($i = 0; $i < 20000; $i++) {
    if (!$cache->store('same', ['i' => $i, 'payload' => str_repeat('x', 1024)])) {
        echo "same failed at $i\n";
        exit;
    }
}

$same = $cache->fetch('same');
var_dump($same['i']);
var_dump(strlen($same['payload']));

for ($i = 0; $i < 5000; $i++) {
    if (!$cache->delete('same')) {
        echo "delete failed at $i\n";
        exit;
    }
    if (!$cache->store('same', ['i' => $i])) {
        echo "store failed at $i\n";
        exit;
    }
}

$churned = $cache->fetch('same');
var_dump($churned['i']);
?>
--EXPECT--
int(19999)
int(1024)
int(4999)
