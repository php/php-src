--TEST--
UserCache\Cache: fetchMultiple normalizes integer keys
--INI--
user_cache.enable=1
user_cache.enable_cli=1
opcache.file_cache_only=0
user_cache.shm_size=16M
--FILE--
<?php
$cache = UserCache\Cache::getPool('fetch-multiple-int');

var_dump($cache->storeMultiple([
    123 => 'int-key',
    '456' => 'numeric-string-key',
]));

$result = $cache->fetchMultiple([123, '456', 789], 'default');
var_dump($result);
var_dump(array_key_exists(123, $result));
var_dump(array_key_exists(456, $result));
var_dump($result[123], $result[456], $result[789]);
?>
--EXPECT--
bool(true)
array(3) {
  [123]=>
  string(7) "int-key"
  [456]=>
  string(18) "numeric-string-key"
  [789]=>
  string(7) "default"
}
bool(true)
bool(true)
string(7) "int-key"
string(18) "numeric-string-key"
string(7) "default"
