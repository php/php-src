--TEST--
UserCache\Cache: missing private property declaring class fails decode
--EXTENSIONS--
user_cache
--INI--
user_cache.enable=1
user_cache.enable_cli=1
opcache.file_cache_only=0
user_cache.shm_size=16M
--FILE--
<?php
#[AllowDynamicProperties]
class UserCacheMissingScopeBox
{
}

$payload = 'O:24:"UserCacheMissingScopeBox":1:{s:15:"' . "\0Missing\0secret" . '";i:42;}';
$value = unserialize($payload);

$cache = UserCache\Cache::getPool('private-property-missing-scope');
$cache->clear();

var_dump($cache->store('value', $value));
var_dump($cache->fetch('value', 'default'));
var_dump($cache->has('value'));
?>
--EXPECT--
bool(true)
string(7) "default"
bool(false)
