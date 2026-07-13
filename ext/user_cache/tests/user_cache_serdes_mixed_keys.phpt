--TEST--
UserCache\Cache: serdes arrays keep mixed string/integer keys intact
--EXTENSIONS--
user_cache
--INI--
user_cache.enable=1
user_cache.enable_cli=1
opcache.file_cache_only=0
user_cache.shm_size=16M
--FILE--
<?php
$cache = UserCache\Cache::getPool('serdes-mixed-keys');

class MixedKeyHolder
{
    public array $data = [];

    public function __wakeup(): void {}
}

$cases = [
    'string then int' => ['name' => 'x', 5 => 'y'],
    'strings then int' => ['a' => 1, 'b' => 2, 10 => 3],
    'int then string' => [5 => 'y', 'name' => 'x'],
    'interleaved' => ['a' => 1, 3 => 'x', 'b' => 2, 7 => 'y'],
    'packed' => ['a', 'b'],
    'nested mixed' => ['outer' => [1 => 'one', 'two' => 2], 9 => ['k' => 'v']],
];

foreach ($cases as $label => $data) {
    $holder = new MixedKeyHolder();
    $holder->data = $data;

    var_dump($cache->store($label, $holder));
    $fetched = $cache->fetch($label);
    var_dump($fetched->data === $data);
}
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
