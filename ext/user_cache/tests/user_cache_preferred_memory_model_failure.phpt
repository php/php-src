--TEST--
UserCache\Cache: a preferred memory model that fails is reported once with the model used instead, and the cache keeps working
--SKIPIF--
<?php
if (!PHP_DEBUG) die('skip requires a debug build (fault injection is ZEND_DEBUG-only)');
if (getenv('SKIP_REPEAT')) die('skip the memory model is chosen once per process');
?>
--ENV--
USER_CACHE_DEBUG_FAIL_PREFERRED_MEMORY_MODEL=1
--INI--
user_cache.enable=1
user_cache.enable_cli=1
user_cache.preferred_memory_model=mmap
--FILE--
<?php
$cache = UserCache\Cache::getPool('preferred-memory-model-failure');
var_dump($cache->store('k', 1), $cache->fetch('k'));
var_dump(UserCache\Cache::getPool('preferred-memory-model-failure-2')->store('k', 2));
?>
--EXPECTF--
Warning: %s: UserCache: preferred memory model "mmap" failed, using "%s": debug fault: %s (0) in %s on line %d
bool(true)
int(1)
bool(true)
