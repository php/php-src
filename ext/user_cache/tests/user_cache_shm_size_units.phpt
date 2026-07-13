--TEST--
UserCache\Cache: shm size directive accepts PHP quantity syntax
--EXTENSIONS--
user_cache
--INI--
user_cache.enable=1
user_cache.enable_cli=1
opcache.file_cache_only=0
user_cache.shm_size=16
--FILE--
<?php
var_dump(UserCache\Cache::getStatus()->getConfiguredMemory());
?>
--EXPECT--
int(16)
