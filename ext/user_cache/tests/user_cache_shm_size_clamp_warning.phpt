--TEST--
UserCache\Cache: shm size clamp emits a PHP warning
--EXTENSIONS--
user_cache
--INI--
user_cache.enable=1
user_cache.enable_cli=1
opcache.file_cache_only=0
opcache.log_verbosity_level=1
user_cache.shm_size=8192M
--FILE--
<?php
var_dump(UserCache\Cache::getStatus()->getConfiguredMemory() > 0);
?>
--EXPECTF--
Warning: user_cache.shm_size is limited to slightly under 4096M; clamping in Unknown on line 0
bool(true)
