--TEST--
UserCache\Cache: classes declaring both magic methods use their serialization contract
--EXTENSIONS--
user_cache
--INI--
user_cache.enable=1
user_cache.enable_cli=1
opcache.file_cache_only=0
user_cache.shm_size=16M
--FILE--
<?php
$cache = UserCache\Cache::getPool('graph');

class MagicUserCacheObject {
    public int $value = 1;
    public static int $calls = 0;

    public function __serialize(): array {
        self::$calls++;
        return ['value' => 99];
    }

    public function __unserialize(array $data): void {
        self::$calls += 10;
        $this->value = $data['value'];
    }
}

$object = new MagicUserCacheObject();
var_dump($cache->store('object', $object));
$fetched = $cache->fetch('object');
var_dump($fetched instanceof MagicUserCacheObject);
var_dump($fetched->value);
var_dump(MagicUserCacheObject::$calls);
?>
--EXPECT--
bool(true)
bool(true)
int(99)
int(11)
