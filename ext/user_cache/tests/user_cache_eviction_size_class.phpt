--TEST--
UserCache\Cache: LRU eviction absorbs a block size-class step without wiping
--INI--
user_cache.enable=1
user_cache.enable_cli=1
opcache.file_cache_only=0
user_cache.shm_size=4M
--FILE--
<?php
$cache = UserCache\Cache::getPool('sizeclass');

/* Fill the segment with one value size class. */
$small = str_repeat('a', 8192);
for ($i = 0; $i < 600; $i++) {
    $cache->store('s' . $i, $small);
}

/* Churn with values two alignment steps larger: every LRU victim frees a
 * block strictly smaller than the incoming store needs, so eviction must
 * keep going until a fitting region exists instead of falling back to a
 * full wipe. */
$large = str_repeat('b', 8192 + 16);
$ok = 0;
for ($i = 0; $i < 300; $i++) {
    if ($cache->store('l' . $i, $large)) {
        $ok++;
    }
}

$status = UserCache\Cache::getStatus();
var_dump($ok === 300);
var_dump($status->getEvictionCount() > 0);
var_dump($status->getExpungeCount() === 0);
var_dump($cache->fetch('l299') === $large);
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
