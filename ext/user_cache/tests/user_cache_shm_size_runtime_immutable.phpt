--TEST--
UserCache\Cache: user_cache.shm_size cannot be changed at runtime
--INI--
user_cache.enable=1
user_cache.enable_cli=1
opcache.file_cache_only=0
user_cache.shm_size=16M
--FILE--
<?php
var_dump(ini_set('user_cache.shm_size', '32M'));
var_dump(ini_get('user_cache.shm_size'));
var_dump(UserCache\Cache::getStatus()->getConfiguredMemory());
?>
--EXPECT--
bool(false)
string(3) "16M"
int(16777216)
