--TEST--
UserCache\Cache: default-valued properties are stored when they carry aliases
--EXTENSIONS--
user_cache
--INI--
user_cache.enable=1
user_cache.enable_cli=1
opcache.file_cache_only=0
user_cache.shm_size=16M
--FILE--
<?php
class UserCacheDefaultAlias
{
    public int $left = 1;
    public int $right = 1;
}

$cache = UserCache\Cache::getPool('default-alias');
$object = new UserCacheDefaultAlias();
$object->left =& $object->right;

var_dump($cache->store('object', $object));

$fetched = $cache->fetch('object');
$fetched->left = 99;
var_dump($fetched->left, $fetched->right);
?>
--EXPECT--
bool(true)
int(99)
int(99)
