--TEST--
UserCache\Cache: remember() failure paths, lock ownership and by-reference callback results
--INI--
user_cache.enable=1
user_cache.enable_cli=1
user_cache.shm_size=16M
--FILE--
<?php
/* A throwing callback or an unstorable result leaves no entry and no held lock. */
$cache = UserCache\Cache::getPool('remember-failure');

try {
    $cache->remember('boom', function () {
        throw new RuntimeException('from callback');
    });
} catch (RuntimeException $e) {
    echo $e->getMessage(), "\n";
}
var_dump($cache->has('boom'));
var_dump($cache->lock('boom'));
var_dump($cache->unlock('boom'));

foreach ([
    'res' => fn() => fopen('php://memory', 'r'),
    'closure' => fn() => fn() => 1,
    'nested-res' => fn() => ['inner' => fopen('php://memory', 'r')],
] as $key => $callback) {
    try {
        $cache->remember($key, $callback);
    } catch (TypeError $e) {
        echo $e->getMessage(), "\n";
    }
    var_dump($cache->has($key));
}

/* remember() keeps a lock the caller already holds and takes none of its own otherwise. */
$cache = UserCache\Cache::getPool('remember-preheld');
$cache->clear();

var_dump($cache->lock('key'));
var_dump($cache->remember('key', fn () => 'value'));
var_dump($cache->fetch('key'));
var_dump($cache->unlock('key'));

var_dump($cache->lock('throwing'));
try {
    $cache->remember('throwing', function () {
        throw new RuntimeException('boom');
    });
} catch (RuntimeException $e) {
    echo $e->getMessage(), "\n";
}
var_dump($cache->unlock('throwing'));

var_dump($cache->lock('unstorable'));
try {
    $cache->remember('unstorable', fn () => fn () => 1);
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
var_dump($cache->unlock('unstorable'));

var_dump($cache->remember('own', fn () => 'own-value'));
var_dump($cache->unlock('own'));

/* A by-reference callback return is unwrapped into a detached plain value. */
$cache = UserCache\Cache::getPool('remember-by-ref');
$cache->clear();

$g = 42;
$byRef = function &(string $key) use (&$g) {
    return $g;
};

$computed = [$cache->remember('by-ref', $byRef)];
var_dump($computed);
var_dump($cache->fetch('by-ref'));

$cached = [$cache->remember('by-ref', $byRef)];
var_dump($cached);

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
from callback
bool(false)
bool(true)
bool(true)
Resources cannot be stored in the user cache
bool(false)
Closure objects cannot be stored in the user cache
bool(false)
Resources cannot be stored in the user cache
bool(false)
bool(true)
string(5) "value"
string(5) "value"
bool(true)
bool(true)
boom
bool(true)
bool(true)
Closure objects cannot be stored in the user cache
bool(true)
string(9) "own-value"
bool(false)
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
