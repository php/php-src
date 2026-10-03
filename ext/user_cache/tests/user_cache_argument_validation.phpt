--TEST--
UserCache\Cache: invalid keys, pool names, negative TTLs and negative leases throw ValueError
--INI--
user_cache.enable=1
user_cache.enable_cli=1
user_cache.shm_size=16M
--FILE--
<?php
/* Invalid cache keys are rejected consistently across single- and multi-key APIs. */
$cache = UserCache\Cache::getPool('invalid-keys');

function show_error(string $label, Closure $callback): void
{
    try {
        $callback();
        echo $label, ": no error\n";
    } catch (Throwable $e) {
        $message = preg_replace('/^[^(]+\\(\\): /', '', $e->getMessage());
        echo $label, ': ', $e::class, ': ', $message, "\n";
    }
}

show_error('store empty', fn() => $cache->store('', 1));
show_error('fetch delimiter', fn() => $cache->fetch("bad\x1fkey"));
show_error('has empty', fn() => $cache->has(''));
show_error('lock delimiter', fn() => $cache->lock("bad\x1fkey"));
show_error('fetchMultiple empty', fn() => $cache->fetchMultiple(['ok', '']));
show_error('fetchMultiple type', fn() => $cache->fetchMultiple(['ok', 1.5]));
show_error('deleteMultiple type', fn() => $cache->deleteMultiple(['ok', new stdClass()]));
show_error('storeMultiple empty key', fn() => $cache->storeMultiple(['' => 1]));
show_error('storeMultiple delimiter key', fn() => $cache->storeMultiple(["bad\x1fkey" => 1]));

/* Invalid pool names are rejected by the pool factory. */
try {
    UserCache\Cache::getPool('');
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}
try {
    UserCache\Cache::getPool("bad\x1fpool");
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}
try {
    UserCache\Cache::deletePool("bad\x1fname");
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}
try {
    UserCache\Cache::deletePool('');
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}

/* A negative TTL throws on every write API and nothing is written. */
$cache = UserCache\Cache::getPool('negative-ttl');

$calls = [
    'store' => fn() => $cache->store('key', 1, -1),
    'add' => fn() => $cache->add('key', 1, -1),
    'storeMultiple' => fn() => $cache->storeMultiple(['key' => 1], -1),
    'increment' => fn() => $cache->increment('key', 1, -1),
    'decrement' => fn() => $cache->decrement('key', 1, -1),
    'remember' => fn() => $cache->remember('key', fn() => 1, -1),
];

foreach ($calls as $fn) {
    try {
        $fn();
    } catch (ValueError $e) {
        echo $e->getMessage(), "\n";
    }
}

var_dump($cache->has('key'));

/* A negative lock lease throws. */
$cache = UserCache\Cache::getPool('negative-lease');

try {
    $cache->lock('key', -1);
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
store empty: ValueError: Argument #1 ($key) must be a non-empty string
fetch delimiter: ValueError: Argument #1 ($key) must not contain the user-cache key delimiter 0x1F
has empty: ValueError: Argument #1 ($key) must be a non-empty string
lock delimiter: ValueError: Argument #1 ($key) must not contain the user-cache key delimiter 0x1F
fetchMultiple empty: ValueError: Argument #1 ($keys) must contain only non-empty string or int cache keys that do not contain 0x1F
fetchMultiple type: ValueError: Argument #1 ($keys) must contain only non-empty string or int cache keys that do not contain 0x1F
deleteMultiple type: ValueError: Argument #1 ($keys) must contain only non-empty string or int cache keys that do not contain 0x1F
storeMultiple empty key: ValueError: Argument #1 ($values) must be an array with non-empty string or int keys that do not contain 0x1F
storeMultiple delimiter key: ValueError: Argument #1 ($values) must be an array with non-empty string or int keys that do not contain 0x1F
UserCache\Cache::getPool(): Argument #1 ($pool) must not be empty
UserCache\Cache::getPool(): Argument #1 ($pool) must not contain the user-cache key delimiter 0x1F
UserCache\Cache::deletePool(): Argument #1 ($pool) must not contain the user-cache key delimiter 0x1F
UserCache\Cache::deletePool(): Argument #1 ($pool) must not be empty
UserCache\Cache::store(): Argument #3 ($ttl) must be greater than or equal to 0
UserCache\Cache::add(): Argument #3 ($ttl) must be greater than or equal to 0
UserCache\Cache::storeMultiple(): Argument #2 ($ttl) must be greater than or equal to 0
UserCache\Cache::increment(): Argument #3 ($ttl) must be greater than or equal to 0
UserCache\Cache::decrement(): Argument #3 ($ttl) must be greater than or equal to 0
UserCache\Cache::remember(): Argument #3 ($ttl) must be greater than or equal to 0
bool(false)
UserCache\Cache::lock(): Argument #2 ($lease) must be greater than or equal to 0
