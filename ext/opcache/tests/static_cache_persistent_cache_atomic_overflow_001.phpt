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

OPcache\pinned_clear();

$maxKey = 'atomic_overflow_max_' . getmypid();
$minKey = 'atomic_overflow_min_' . getmypid();
$missingKey = 'atomic_overflow_missing_' . getmypid();
$handlerProbeKey = 'atomic_overflow_handler_probe_' . getmypid();

OPcache\pinned_store($handlerProbeKey, 'ok');

set_error_handler(static function (int $severity, string $message) use ($handlerProbeKey): bool {
	echo "warning severity: ";
	var_dump($severity === E_WARNING);
	echo "warning message: {$message}\n";
	echo "handler fetch: ";
	var_dump(OPcache\pinned_fetch($handlerProbeKey) === 'ok');

	return true;
});

var_dump(OPcache\pinned_store($maxKey, PHP_INT_MAX));
show_result('increment max no throw', static fn () => OPcache\pinned_atomic_increment($maxKey), PHP_INT_MIN);
echo "max wrapped no throw: ";
var_dump(OPcache\pinned_fetch($maxKey) === PHP_INT_MIN);

var_dump(OPcache\pinned_store($maxKey, PHP_INT_MAX));
show_result('increment max throw flag', static fn () => OPcache\pinned_atomic_increment($maxKey, 1, true), PHP_INT_MIN);
echo "max wrapped throw flag: ";
var_dump(OPcache\pinned_fetch($maxKey) === PHP_INT_MIN);

var_dump(OPcache\pinned_store($minKey, PHP_INT_MIN));
show_result('decrement min no throw', static fn () => OPcache\pinned_atomic_decrement($minKey), PHP_INT_MAX);
echo "min wrapped no throw: ";
var_dump(OPcache\pinned_fetch($minKey) === PHP_INT_MAX);

var_dump(OPcache\pinned_store($minKey, PHP_INT_MIN));
show_result('decrement min throw flag', static fn () => OPcache\pinned_atomic_decrement($minKey, 1, true), PHP_INT_MAX);
echo "min wrapped throw flag: ";
var_dump(OPcache\pinned_fetch($minKey) === PHP_INT_MAX);

show_result('decrement missing min step', static fn () => OPcache\pinned_atomic_decrement($missingKey, PHP_INT_MIN), PHP_INT_MIN);
echo "missing wrapped: ";
var_dump(OPcache\pinned_fetch($missingKey) === PHP_INT_MIN);

?>
--EXPECT--
bool(true)
-- increment max no throw --
warning severity: bool(true)
warning message: OPcache\pinned_atomic_increment(): Integer overflow occurred; result wrapped around
handler fetch: bool(true)
result: bool(true)
max wrapped no throw: bool(true)
bool(true)
-- increment max throw flag --
warning severity: bool(true)
warning message: OPcache\pinned_atomic_increment(): Integer overflow occurred; result wrapped around
handler fetch: bool(true)
result: bool(true)
max wrapped throw flag: bool(true)
bool(true)
-- decrement min no throw --
warning severity: bool(true)
warning message: OPcache\pinned_atomic_decrement(): Integer overflow occurred; result wrapped around
handler fetch: bool(true)
result: bool(true)
min wrapped no throw: bool(true)
bool(true)
-- decrement min throw flag --
warning severity: bool(true)
warning message: OPcache\pinned_atomic_decrement(): Integer overflow occurred; result wrapped around
handler fetch: bool(true)
result: bool(true)
min wrapped throw flag: bool(true)
-- decrement missing min step --
warning severity: bool(true)
warning message: OPcache\pinned_atomic_decrement(): Integer overflow occurred; result wrapped around
handler fetch: bool(true)
result: bool(true)
missing wrapped: bool(true)
