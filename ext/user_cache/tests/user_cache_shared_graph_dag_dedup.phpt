--TEST--
UserCache\Cache: repeated arrays in a DAG round-trip with copy-on-write isolation
--INI--
user_cache.enable=1
user_cache.enable_cli=1
opcache.file_cache_only=0
user_cache.shm_size=16M
--FILE--
<?php
$cache = UserCache\Cache::getPool('dag-dedup');

$shared = [
    'text' => str_repeat('q', 64),
    'list' => range(1, 32),
    'nested' => ['deep' => 'value'],
];

$dag = [
    'first' => $shared,
    'second' => $shared,
    'third' => $shared,
    'wrapped' => [$shared, $shared],
];

var_dump($cache->store('dag', $dag));
$fetched = $cache->fetch('dag');
var_dump($fetched === $dag);

$fetched['first']['nested']['deep'] = 'mutated';
var_dump($fetched['second']['nested']['deep']);
var_dump($fetched['wrapped'][1] === $shared);

$pair = [['k' => 'v'], ['k' => 'v']];
var_dump($cache->store('pair', $pair));
var_dump($cache->fetch('pair') === $pair);

$dag['third']['extra'] = 'tail';
var_dump($cache->store('dag', $dag));
var_dump($cache->fetch('dag') === $dag);
?>
--EXPECT--
bool(true)
bool(true)
string(5) "value"
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
