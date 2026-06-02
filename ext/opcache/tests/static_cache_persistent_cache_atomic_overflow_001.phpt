--TEST--
OPcache pinned atomic operations warn and wrap on integer overflow
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.static_cache.pinned_size_mb=32
--FILE--
<?php

function show_result(string $label, callable $callback, int $expected): void
{
	echo "-- {$label} --\n";
	$result = $callback();
	echo "result: ";
	var_dump($result === $expected);
}

OPcache\PinnedCache::clear();

$maxKey = 'atomic_overflow_max_' . getmypid();
$minKey = 'atomic_overflow_min_' . getmypid();
$missingKey = 'atomic_overflow_missing_' . getmypid();
$handlerProbeKey = 'atomic_overflow_handler_probe_' . getmypid();

OPcache\PinnedCache::set($handlerProbeKey, 'ok');

set_error_handler(static function (int $severity, string $message) use ($handlerProbeKey): bool {
	echo "warning severity: ";
	var_dump($severity === E_WARNING);
	echo "warning message: {$message}\n";
	echo "handler fetch: ";
	var_dump(OPcache\PinnedCache::get($handlerProbeKey) === 'ok');

	return true;
});

var_dump(OPcache\PinnedCache::set($maxKey, PHP_INT_MAX));
show_result('increment max', static fn () => OPcache\PinnedCache::increment($maxKey), PHP_INT_MIN);
echo "max wrapped: ";
var_dump(OPcache\PinnedCache::get($maxKey) === PHP_INT_MIN);

var_dump(OPcache\PinnedCache::set($minKey, PHP_INT_MIN));
show_result('decrement min', static fn () => OPcache\PinnedCache::decrement($minKey), PHP_INT_MAX);
echo "min wrapped: ";
var_dump(OPcache\PinnedCache::get($minKey) === PHP_INT_MAX);

show_result('decrement missing min step', static fn () => OPcache\PinnedCache::decrement($missingKey, PHP_INT_MIN), PHP_INT_MIN);
echo "missing wrapped: ";
var_dump(OPcache\PinnedCache::get($missingKey) === PHP_INT_MIN);

?>
--EXPECT--
bool(true)
-- increment max --
warning severity: bool(true)
warning message: OPcache\PinnedCache::increment(): Integer overflow occurred; result wrapped around
handler fetch: bool(true)
result: bool(true)
max wrapped: bool(true)
bool(true)
-- decrement min --
warning severity: bool(true)
warning message: OPcache\PinnedCache::decrement(): Integer overflow occurred; result wrapped around
handler fetch: bool(true)
result: bool(true)
min wrapped: bool(true)
-- decrement missing min step --
warning severity: bool(true)
warning message: OPcache\PinnedCache::decrement(): Integer overflow occurred; result wrapped around
handler fetch: bool(true)
result: bool(true)
missing wrapped: bool(true)
