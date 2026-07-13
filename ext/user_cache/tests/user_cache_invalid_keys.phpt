--TEST--
UserCache\Cache: invalid cache keys are rejected consistently
--EXTENSIONS--
user_cache
--INI--
user_cache.enable=1
user_cache.enable_cli=1
opcache.file_cache_only=0
user_cache.shm_size=16M
--FILE--
<?php
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
