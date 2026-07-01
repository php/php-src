--TEST--
OPcache User Cache: rehashes the entry table after delete churn
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_cache_only=0
opcache.user_cache_shm_size=16M
--FILE--
<?php
$cache = new Opcache\UserCache('rehash');

for ($i = 0; $i < 50; $i++) {
    $cache->store("keep_$i", ['id' => $i, 'name' => "keeper_$i"]);
}

for ($round = 0; $round < 10; $round++) {
    for ($i = 0; $i < 100; $i++) {
        $cache->store("churn_{$round}_{$i}", $i);
    }
    for ($i = 0; $i < 100; $i++) {
        $cache->delete("churn_{$round}_{$i}");
    }
}

$ok = true;
for ($i = 0; $i < 50; $i++) {
    $value = $cache->fetch("keep_$i");
    if (!is_array($value) || $value['id'] !== $i || $value['name'] !== "keeper_$i") {
        $ok = false;
        echo "lost keep_$i\n";
    }
}

var_dump($ok);
var_dump($cache->fetch('churn_0_0', 'gone'));
var_dump($cache->has('churn_9_99'));
var_dump($cache->store('after_rehash', 'value'));
var_dump($cache->fetch('after_rehash'));
?>
--EXPECT--
bool(true)
string(4) "gone"
bool(false)
bool(true)
string(5) "value"
