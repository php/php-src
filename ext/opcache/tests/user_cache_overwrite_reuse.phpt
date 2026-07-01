--TEST--
OPcache User Cache: repeated stores reuse existing entries
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_cache_only=0
opcache.user_cache_shm_size=16M
--FILE--
<?php
$cache = new Opcache\UserCache('overwrite-reuse');

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
