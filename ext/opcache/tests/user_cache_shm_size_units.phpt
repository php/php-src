--TEST--
OPcache User Cache: shm size directive accepts PHP quantity syntax
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_cache_only=0
opcache.user_cache_shm_size=16
--FILE--
<?php
$config = opcache_get_configuration();
var_dump($config['directives']['opcache.user_cache_shm_size']);
?>
--EXPECT--
int(16)
