--TEST--
UserCache\Cache: deletePool() removes a pool and its values, leaving others intact
--EXTENSIONS--
user_cache
--INI--
user_cache.enable=1
user_cache.enable_cli=1
opcache.file_cache_only=0
user_cache.shm_size=16M
--FILE--
<?php
$a = UserCache\Cache::getPool('delete-pool-a');
$b = UserCache\Cache::getPool('delete-pool-b');

$a->store('k', 'value-a');
$b->store('k', 'value-b');

var_dump(UserCache\Cache::hasPool('delete-pool-a'));
var_dump($a->fetch('k', 'MISS'));

echo "--- deletePool('delete-pool-a') ---\n";
var_dump(UserCache\Cache::deletePool('delete-pool-a'));

/* The pool is gone from the registry and getPools(). */
var_dump(UserCache\Cache::hasPool('delete-pool-a'));
var_dump(array_keys(UserCache\Cache::getPools()));

/* Its values are gone: a fresh getPool() sees an empty pool. */
$a2 = UserCache\Cache::getPool('delete-pool-a');
var_dump($a2->fetch('k', 'MISS'));
var_dump($a2->has('k'));

/* The other pool is untouched. */
var_dump($b->fetch('k', 'MISS'));

echo "--- deleting an unknown pool is a no-op success ---\n";
var_dump(UserCache\Cache::deletePool('never-created'));

echo "--- invalid pool names throw ---\n";
try {
    UserCache\Cache::deletePool("bad\x1fname");
} catch (\ValueError $e) {
    echo $e->getMessage(), "\n";
}
try {
    UserCache\Cache::deletePool('');
} catch (\ValueError $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
bool(true)
string(7) "value-a"
--- deletePool('delete-pool-a') ---
bool(true)
bool(false)
array(1) {
  [0]=>
  string(13) "delete-pool-b"
}
string(4) "MISS"
bool(false)
string(7) "value-b"
--- deleting an unknown pool is a no-op success ---
bool(true)
--- invalid pool names throw ---
UserCache\Cache::deletePool(): Argument #1 ($pool) must not contain the user-cache key delimiter 0x1F
UserCache\Cache::deletePool(): Argument #1 ($pool) must not be empty
