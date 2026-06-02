--TEST--
OPcache explicit volatile and pinned delete frees payload memory across requests
--EXTENSIONS--
opcache
--CONFLICTS--
server
--FILE--
<?php

file_put_contents(__DIR__ . '/explicit_cache_store_delete_request_reuse_001.php', <<<'PHP'
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

function run_action(string $kind, string $action): void
{
    $prefix = $kind . '_request_reuse_';

    if ($action === 'seed') {
        cache_clear($kind);
        var_dump(cache_store($kind, $prefix . 'first', str_repeat('A', 1800000)));
        var_dump(cache_store($kind, $prefix . 'second', str_repeat('B', 1800000)));
        var_dump(cache_store($kind, $prefix . 'third', str_repeat('C', 1800000)));
        return;
    }

    if ($action === 'delete') {
        cache_delete($kind, $prefix . 'second');
        var_dump(cache_fetch($kind, $prefix . 'second'));
        return;
    }

    if ($action === 'refill') {
        var_dump(cache_store($kind, $prefix . 'replacement', str_repeat('R', 1500000)));
        return;
    }

    var_dump(strlen(cache_fetch($kind, $prefix . 'first')));
    var_dump(strlen(cache_fetch($kind, $prefix . 'third')));
    var_dump(strlen(cache_fetch($kind, $prefix . 'replacement')));
}

$kind = $_GET['kind'];
$action = $_GET['action'];
run_action($kind, $action);
PHP);

$php = getenv('TEST_PHP_EXECUTABLE');
if ($php) {
    $php = realpath(__DIR__ . '/../../../' . $php) ?: $php;
    putenv('TEST_PHP_EXECUTABLE=' . $php);
}

include 'php_cli_server.inc';
php_cli_server_start('-d opcache.enable=1 -d opcache.enable_cli=1 -d opcache.static_cache.volatile_size_mb=8 -d opcache.static_cache.pinned_size_mb=8 -d opcache.file_update_protection=0');

$base = 'http://' . PHP_CLI_SERVER_ADDRESS . '/explicit_cache_store_delete_request_reuse_001.php';
foreach (['volatile', 'pinned'] as $kind) {
    echo "-- {$kind} --\n";
    echo file_get_contents($base . '?kind=' . $kind . '&action=seed');
    echo file_get_contents($base . '?kind=' . $kind . '&action=delete');
    echo file_get_contents($base . '?kind=' . $kind . '&action=refill');
    echo file_get_contents($base . '?kind=' . $kind . '&action=read');
}

?>
--CLEAN--
<?php
@unlink(__DIR__ . '/explicit_cache_store_delete_request_reuse_001.php');
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
-- pinned --
bool(true)
bool(true)
bool(true)
string(7) "missing"
bool(true)
int(1800000)
int(1800000)
int(1500000)
