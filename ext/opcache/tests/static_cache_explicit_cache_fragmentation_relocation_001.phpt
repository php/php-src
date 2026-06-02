--TEST--
OPcache explicit volatile and pinned caches relocate fragmented payload blocks before store failure
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.static_cache.volatile_size_mb=8
opcache.static_cache.pinned_size_mb=8
--FILE--
<?php

function cache_clear(string $kind): void
{
    if ($kind === 'volatile') {
        OPcache\VolatileCache::clear();
    } else {
        OPcache\PinnedCache::clear();
    }
}

function cache_store(string $kind, string $key, string $value): bool
{
    if ($kind === 'volatile') {
        return OPcache\VolatileCache::set($key, $value);
    }

    OPcache\PinnedCache::set($key, $value);
    return true;
}

function cache_fetch(string $kind, string $key): string
{
    return $kind === 'volatile'
        ? OPcache\VolatileCache::get($key, 'missing')
        : OPcache\PinnedCache::get($key, 'missing');
}

function cache_delete(string $kind, string $key): void
{
    if ($kind === 'volatile') {
        OPcache\VolatileCache::delete($key);
    } else {
        OPcache\PinnedCache::delete($key);
    }
}

function run_fragmentation_relocation(string $kind): void
{
    echo "-- {$kind} --\n";

    cache_clear($kind);

    $prefix = $kind . '_fragmentation_';
    var_dump(cache_store($kind, $prefix . 'first', str_repeat('A', 1200000)));
    var_dump(cache_store($kind, $prefix . 'second', str_repeat('B', 1200000)));
    var_dump(cache_store($kind, $prefix . 'third', str_repeat('C', 1200000)));
    var_dump(cache_store($kind, $prefix . 'fourth', str_repeat('D', 1200000)));

    cache_delete($kind, $prefix . 'second');

    var_dump(cache_store($kind, $prefix . 'merged', str_repeat('M', 2400000)));
    var_dump(strlen(cache_fetch($kind, $prefix . 'merged')));
    var_dump(strlen(cache_fetch($kind, $prefix . 'first')));
    var_dump(strlen(cache_fetch($kind, $prefix . 'third')));
    var_dump(strlen(cache_fetch($kind, $prefix . 'fourth')));
}

run_fragmentation_relocation('volatile');
run_fragmentation_relocation('pinned');

?>
--EXPECT--
-- volatile --
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
int(2400000)
int(1200000)
int(1200000)
int(1200000)
-- pinned --
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
int(2400000)
int(1200000)
int(1200000)
int(1200000)
