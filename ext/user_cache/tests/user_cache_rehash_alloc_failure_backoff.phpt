--TEST--
UserCache\Cache: after the entry table rehash fails to allocate its snapshot, the following writes skip the rehash for a while instead of retrying on every write, and a later write rehashes
--SKIPIF--
<?php
if (!PHP_DEBUG) die('skip requires a debug build (fault injection is ZEND_DEBUG-only)');
?>
--INI--
user_cache.enable=1
user_cache.enable_cli=1
user_cache.entries_hint=127
--FILE--
<?php
$cache = UserCache\Cache::getPool('rehash-alloc-failure');
$cache->clear();

putenv('USER_CACHE_DEBUG_FAIL_REHASH_ALLOC=1');
for ($i = 0; $i < 100; $i++) {
    $cache->store("k$i", $i);
}
for ($i = 0; $i < 60; $i++) {
    $cache->delete("k$i");
}
var_dump(UserCache\Cache::getStatus()->getTombstoneCount());
putenv('USER_CACHE_DEBUG_FAIL_REHASH_ALLOC');

/* Overwrites with strings take the write lock and keep the tombstones, so only a rehash can clear them. */
for ($i = 0; $i < 10; $i++) {
    $cache->store('k99', "v$i");
}
var_dump(UserCache\Cache::getStatus()->getTombstoneCount());

for ($i = 0; $i < 100; $i++) {
    $cache->store('k99', "v$i");
}
var_dump(UserCache\Cache::getStatus()->getTombstoneCount(), $cache->fetch('k99'), $cache->fetch('k98'), $cache->has('k0'));
?>
--EXPECT--
int(60)
int(60)
int(0)
string(3) "v99"
int(98)
bool(false)
