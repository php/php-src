--TEST--
OPcache User Cache: shared graph does not call magic serialization methods
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_cache_only=0
opcache.user_cache_shm_size=16M
--FILE--
<?php
$cache = new Opcache\UserCache('graph');

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
int(1)
int(0)
