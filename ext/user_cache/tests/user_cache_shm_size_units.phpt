--TEST--
UserCache\Cache: shm size directive accepts PHP quantity syntax
--INI--
user_cache.enable=1
user_cache.enable_cli=1
opcache.file_cache_only=0
user_cache.shm_size=16
--FILE--
<?php
var_dump(UserCache\Cache::getStatus()->getConfiguredMemory());
?>
--EXPECTF--
Warning: user_cache.shm_size (16) cannot hold the minimum cache layout (%d bytes); the cache will be unavailable in Unknown on line 0
int(16)
