--TEST--
UserCache\Cache: remember() unwraps a by-reference callback return into a plain value
--INI--
user_cache.enable=1
user_cache.enable_cli=1
opcache.file_cache_only=0
user_cache.shm_size=16M
--FILE--
<?php
$cache = UserCache\Cache::getPool('remember-by-ref');

/* Drop leftovers so a --repeat re-run in the same process misses again. */
$cache->delete('by-ref');
$cache->delete('plain');

$g = 42;
$byRef = function &(string $key) use (&$g) {
    return $g;
};

/* Wrapped in an array so var_dump() would print the reference marker if the
 * IS_REFERENCE returned by the callback leaked out of remember(). */
$computed = [$cache->remember('by-ref', $byRef)];
var_dump($computed);
var_dump($cache->fetch('by-ref'));

/* The second call is a cache hit and must return the same plain value. */
$cached = [$cache->remember('by-ref', $byRef)];
var_dump($cached);

/* The stored value must be a copy, not an alias of the callback's variable. */
$g = 99;
var_dump($cache->fetch('by-ref'));

$calls = 0;
$plain = static function (string $key) use (&$calls): string {
    $calls++;

    return 'computed:' . $key;
};
var_dump($cache->remember('plain', $plain));
var_dump($cache->remember('plain', $plain));
var_dump($cache->fetch('plain'));
var_dump($calls);
?>
--EXPECT--
array(1) {
  [0]=>
  int(42)
}
int(42)
array(1) {
  [0]=>
  int(42)
}
int(42)
string(14) "computed:plain"
string(14) "computed:plain"
string(14) "computed:plain"
int(1)
