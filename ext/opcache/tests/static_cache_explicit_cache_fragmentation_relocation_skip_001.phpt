--TEST--
OPcache explicit caches skip relocation when fragmented free memory still cannot satisfy the store
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.static_cache.volatile_size_mb=8
opcache.static_cache.persistent_size_mb=8
--FILE--
<?php

class ExplicitCacheRelocationSkipProbe
{
    public function __construct(public string $name) {}
}

function cache_clear(string $kind): void
{
    if ($kind === 'volatile') {
        OPcache\volatile_clear();
    } else {
        OPcache\persistent_clear();
    }
}

function cache_store(string $kind, string $key, mixed $value): bool
{
    if ($kind === 'volatile') {
        return OPcache\volatile_store($key, $value);
    }

    OPcache\persistent_store($key, $value);
    return true;
}

function cache_fetch(string $kind, string $key): mixed
{
    return $kind === 'volatile'
        ? OPcache\volatile_fetch($key, 'missing')
        : OPcache\persistent_fetch($key, 'missing');
}

function cache_delete(string $kind, string $key): void
{
    if ($kind === 'volatile') {
        OPcache\volatile_delete($key);
    } else {
        OPcache\persistent_delete($key);
    }
}

function run_relocation_skip(string $kind): void
{
    echo "-- {$kind} --\n";

    $prefix = $kind . '_relocation_skip_';
    cache_clear($kind);

    var_dump(cache_store($kind, $prefix . 'probe', new ExplicitCacheRelocationSkipProbe($kind)));
    var_dump(cache_store($kind, $prefix . 'first', str_repeat('A', 1200000)));
    var_dump(cache_store($kind, $prefix . 'second', str_repeat('B', 1200000)));
    var_dump(cache_store($kind, $prefix . 'third', str_repeat('C', 1200000)));
    var_dump(cache_store($kind, $prefix . 'fourth', str_repeat('D', 1200000)));

    cache_delete($kind, $prefix . 'second');
    $probe = cache_fetch($kind, $prefix . 'probe');

    try {
        var_dump(cache_store($kind, $prefix . 'too-large-for-fragments', str_repeat('H', 4500000)));
    } catch (OPcache\StaticCacheException $e) {
        echo get_class($e), ': ', $e->getMessage(), "\n";
    }

    var_dump(cache_fetch($kind, $prefix . 'probe') === $probe);
    var_dump(strlen(cache_fetch($kind, $prefix . 'first')));
    var_dump(strlen(cache_fetch($kind, $prefix . 'third')));
    var_dump(strlen(cache_fetch($kind, $prefix . 'fourth')));
}

run_relocation_skip('volatile');
run_relocation_skip('persistent');

?>
--EXPECT--
-- volatile --
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(false)
bool(false)
int(1200000)
int(1200000)
int(1200000)
-- persistent --
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
OPcache\StaticCacheException: not enough shared memory left
bool(false)
int(1200000)
int(1200000)
int(1200000)
