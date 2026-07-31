--TEST--
UserCache\Cache: object with an empty property name decodes without crashing
--INI--
user_cache.enable=1
user_cache.enable_cli=1
opcache.file_cache_only=0
user_cache.shm_size=16M
--FILE--
<?php
#[AllowDynamicProperties]
class UserCacheEmptyPropertyNameBox
{
    public int $declared = 1;
}

$cache = UserCache\Cache::getPool('object-empty-property-name');
$cache->clear();

$plain = (object) ['' => 'empty-name'];
var_dump($cache->store('plain', $plain));
var_dump($cache->fetch('plain', 'default'));
var_dump($cache->fetch('plain', 'default'));

$box = new UserCacheEmptyPropertyNameBox();
$box->{''} = 'empty-name';
var_dump($cache->store('box', $box));
var_dump($cache->fetch('box', 'default'));
?>
--EXPECTF--
bool(true)
object(stdClass)#%d (1) {
  [""]=>
  string(10) "empty-name"
}
object(stdClass)#%d (1) {
  [""]=>
  string(10) "empty-name"
}
bool(true)
object(UserCacheEmptyPropertyNameBox)#%d (2) {
  ["declared"]=>
  int(1)
  [""]=>
  string(10) "empty-name"
}
