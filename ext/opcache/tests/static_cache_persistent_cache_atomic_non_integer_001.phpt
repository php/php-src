--TEST--
OPcache stable atomic operations reject non-integer entries
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.static_cache.stable_size_mb=32
--FILE--
<?php

OPcache\StableCache::getInstance('default')->store('text', 'php');

function dump_atomic_failure(string $label, Closure $callback): void
{
	echo $label, ': ';
	try {
		var_dump($callback());
	} catch (Throwable $exception) {
		echo get_class($exception), ': ', $exception->getMessage(), "\n";
	}
}

dump_atomic_failure('increment', static fn () => OPcache\StableCache::getInstance('default')->increment('text'));
dump_atomic_failure('decrement', static fn () => OPcache\StableCache::getInstance('default')->decrement('text'));

?>
--EXPECT--
increment: OPcache\StaticCacheException: OPcache\StableCache::increment(): Unable to update stable cache integer value for key "text"
decrement: OPcache\StaticCacheException: OPcache\StableCache::decrement(): Unable to update stable cache integer value for key "text"
