--TEST--
UserCache\Cache: fetched strings used as regex patterns are not kept as PCRE cache keys
--SKIPIF--
<?php
if (!function_exists('proc_open')) die('skip proc_open() not available');
?>
--FILE--
<?php
$php = getenv('TEST_PHP_EXECUTABLE') ?: PHP_BINARY;
$args = ['-n', '-d', 'user_cache.enable=1', '-d', 'user_cache.enable_cli=1'];
$code = <<<'PHP'
$cache = UserCache\Cache::getPool('pcre_cache_keys');
$cache->store('routes', ['user' => '#^/user/(\d+)$#', 'post' => '#^/post/(\w+)$#']);
$routes = $cache->fetch('routes');
var_dump(preg_match($routes['user'], '/user/42', $m), $m[1]);
var_dump(preg_match($routes['post'], '/post/abc', $m), $m[1]);
$cache->store('pattern', '#^[a-z]+$#');
var_dump(preg_match($cache->fetch('pattern'), 'abc'));
var_dump(preg_match('#^' . str_repeat('a', 3) . '$#', 'aaa'));
PHP;

/* The PCRE cache releases its keys at process shutdown, after the cache
 * segment has been unmapped; the exit status is checked in a child because
 * the crash happens after stdout is closed. */
$process = proc_open([$php, ...$args, '-r', $code], [1 => ['pipe', 'w'], 2 => ['redirect', 1]], $pipes);
echo stream_get_contents($pipes[1]);
fclose($pipes[1]);
var_dump(proc_close($process));
?>
--EXPECT--
int(1)
string(2) "42"
int(1)
string(3) "abc"
int(1)
int(1)
int(0)
