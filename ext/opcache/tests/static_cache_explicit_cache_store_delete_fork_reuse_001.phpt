--TEST--
OPcache explicit volatile and persistent delete frees payload memory across processes
--EXTENSIONS--
opcache
pcntl
--SKIPIF--
<?php
if (!function_exists('pcntl_fork')) {
    die('skip pcntl_fork() not available');
}
?>
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.static_cache.volatile_size_mb=8
opcache.static_cache.persistent_size_mb=8
--FILE--
<?php

function cache_clear(string $kind): void
{
    if ($kind === 'volatile') {
        OPcache\volatile_clear();
    } else {
        OPcache\persistent_clear();
    }
}

function cache_store(string $kind, string $key, string $value): bool
{
    if ($kind === 'volatile') {
        return OPcache\volatile_store($key, $value);
    }

    OPcache\persistent_store($key, $value);
    return true;
}

function cache_fetch(string $kind, string $key): string
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

function run_fork_reuse(string $kind): void
{
    echo "-- {$kind} --\n";

    $prefix = $kind . '_fork_reuse_';
    cache_clear($kind);
    var_dump(cache_store($kind, $prefix . 'first', str_repeat('A', 1800000)));
    var_dump(cache_store($kind, $prefix . 'second', str_repeat('B', 1800000)));
    var_dump(cache_store($kind, $prefix . 'third', str_repeat('C', 1800000)));

    $pid = pcntl_fork();
    if ($pid === -1) {
        throw new RuntimeException('pcntl_fork() failed');
    }

    if ($pid === 0) {
        cache_delete($kind, $prefix . 'second');
        exit(0);
    }

    pcntl_waitpid($pid, $status);
    if (!pcntl_wifexited($status) || pcntl_wexitstatus($status) !== 0) {
        throw new RuntimeException('child failed');
    }

    var_dump(cache_fetch($kind, $prefix . 'second'));
    var_dump(cache_store($kind, $prefix . 'replacement', str_repeat('R', 1500000)));
    var_dump(strlen(cache_fetch($kind, $prefix . 'first')));
    var_dump(strlen(cache_fetch($kind, $prefix . 'third')));
    var_dump(strlen(cache_fetch($kind, $prefix . 'replacement')));
}

run_fork_reuse('volatile');
run_fork_reuse('persistent');

?>
--EXPECT--
-- volatile --
bool(true)
bool(true)
bool(true)
string(7) "missing"
bool(true)
int(1800000)
int(1800000)
int(1500000)
-- persistent --
bool(true)
bool(true)
bool(true)
string(7) "missing"
bool(true)
int(1800000)
int(1800000)
int(1500000)
