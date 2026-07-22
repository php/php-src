--TEST--
UserCache\Cache: fetchMultiple keeps input key order when warm and cold keys mix
--INI--
user_cache.enable=1
user_cache.enable_cli=1
opcache.file_cache_only=0
user_cache.shm_size=16M
--FILE--
<?php
class OrderProbe
{
    public function __construct(public int $n) {}
}

$cache = UserCache\Cache::getPool('fetch-order');
$cache->clear();

var_dump($cache->store('cold', new OrderProbe(1)));
var_dump($cache->store('warm', new OrderProbe(2)));

$cache->fetch('warm');

$result = $cache->fetchMultiple(['cold', 'warm', 'missing'], 'dflt');
var_dump(array_keys($result));
var_dump($result['cold']->n, $result['warm']->n, $result['missing']);
?>
--EXPECT--
bool(true)
bool(true)
array(3) {
  [0]=>
  string(4) "cold"
  [1]=>
  string(4) "warm"
  [2]=>
  string(7) "missing"
}
int(1)
int(2)
string(4) "dflt"
