--TEST--
UserCache\Cache: storeMultiple() rolls back a committed replacement and releases the lock when the commit loop bails out (CLI)
--SKIPIF--
<?php
if (!PHP_DEBUG) die('skip requires a debug build (fault injection is ZEND_DEBUG-only)');
?>
--INI--
user_cache.enable=1
user_cache.enable_cli=1
user_cache.shm_size=16M
--FILE--
<?php
$cache = UserCache\Cache::getPool('bulk-bailout-cli');
$cache->clear();
$cache->store('a', 'ORIGINAL');
$cache->store('b', ['ORIGINAL-B']);
$baseline = UserCache\Cache::getStatus()->getFreeMemory();
$entries = UserCache\Cache::getStatus()->getEntryCount();

register_shutdown_function(function () use ($cache, $baseline, $entries) {
    putenv('USER_CACHE_DEBUG_FORCE_BULK_COMMIT_BAILOUT');

    echo "shutdown\n";
    var_dump($cache->fetch('a', 'MISS'));
    var_dump($cache->fetch('b', 'MISS'));
    var_dump($cache->has('z'));
    var_dump(UserCache\Cache::getStatus()->getFreeMemory() === $baseline);
    var_dump(UserCache\Cache::getStatus()->getEntryCount() === $entries);

    /* The global lock was released: later writes and bulk writes proceed */
    var_dump($cache->store('after', 1));
    var_dump($cache->storeMultiple(['a' => 'REPLACED', 'z' => 'NEW']));
    var_dump($cache->fetchMultiple(['a', 'b', 'z', 'after']));
});

/* The fault fires after the first item ('a') has been committed */
putenv('USER_CACHE_DEBUG_FORCE_BULK_COMMIT_BAILOUT=1');
$cache->storeMultiple(['a' => 'REPLACEMENT', 'z' => 'NEW', 'b' => ['REPLACEMENT-B']]);
echo "not reached\n";
?>
--EXPECT--
shutdown
string(8) "ORIGINAL"
array(1) {
  [0]=>
  string(10) "ORIGINAL-B"
}
bool(false)
bool(true)
bool(true)
bool(true)
bool(true)
array(4) {
  ["a"]=>
  string(8) "REPLACED"
  ["b"]=>
  array(1) {
    [0]=>
    string(10) "ORIGINAL-B"
  }
  ["z"]=>
  string(3) "NEW"
  ["after"]=>
  int(1)
}
