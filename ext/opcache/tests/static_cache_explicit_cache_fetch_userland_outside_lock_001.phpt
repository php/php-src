--TEST--
OPcache explicit cache fetch materializes userland objects outside the cache read lock
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.static_cache.volatile_size_mb=32
opcache.static_cache.stable_size_mb=32
--FILE--
<?php

class ReentrantFetchPayload
{
	public function __construct(private string $backend)
	{
	}

	public function __serialize(): array
	{
		return ['backend' => $this->backend];
	}

	public function __unserialize(array $data): void
	{
		$this->backend = $data['backend'];
		echo "unserialize-", $this->backend, "\n";

		if ($this->backend === 'volatile') {
			OPcache\VolatileCache::getInstance('default')->store('fetch_inner_volatile', 'ok');
		} else {
			OPcache\StableCache::getInstance('default')->store('fetch_inner_stable', 'ok');
		}
	}
}

foreach (['volatile', 'stable'] as $backend) {
	echo $backend, "\n";

	if ($backend === 'volatile') {
		opcache_static_cache_volatile_reset();
		OPcache\VolatileCache::getInstance('default')->store('fetch_payload', new ReentrantFetchPayload($backend));
		var_dump(OPcache\VolatileCache::getInstance('default')->fetch('fetch_payload') instanceof ReentrantFetchPayload);
		var_dump(OPcache\VolatileCache::getInstance('default')->fetch('fetch_inner_volatile'));
	} else {
		OPcache\StableCache::getInstance('default')->clear();
		OPcache\StableCache::getInstance('default')->store('fetch_payload', new ReentrantFetchPayload($backend));
		var_dump(OPcache\StableCache::getInstance('default')->fetch('fetch_payload') instanceof ReentrantFetchPayload);
		var_dump(OPcache\StableCache::getInstance('default')->fetch('fetch_inner_stable'));
	}
}

?>
--EXPECT--
volatile
unserialize-volatile
bool(true)
string(2) "ok"
stable
unserialize-stable
bool(true)
string(2) "ok"
