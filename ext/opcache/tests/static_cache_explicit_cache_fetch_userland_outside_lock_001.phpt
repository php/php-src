--TEST--
OPcache explicit cache fetch materializes userland objects outside the cache read lock
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.static_cache.volatile_size_mb=32
opcache.static_cache.pinned_size_mb=32
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
			OPcache\VolatileCache::set('fetch_inner_volatile', 'ok');
		} else {
			OPcache\PinnedCache::set('fetch_inner_pinned', 'ok');
		}
	}
}

foreach (['volatile', 'pinned'] as $backend) {
	echo $backend, "\n";

	if ($backend === 'volatile') {
		OPcache\VolatileCache::clear();
		OPcache\VolatileCache::set('fetch_payload', new ReentrantFetchPayload($backend));
		var_dump(OPcache\VolatileCache::get('fetch_payload') instanceof ReentrantFetchPayload);
		var_dump(OPcache\VolatileCache::get('fetch_inner_volatile'));
	} else {
		OPcache\PinnedCache::clear();
		OPcache\PinnedCache::set('fetch_payload', new ReentrantFetchPayload($backend));
		var_dump(OPcache\PinnedCache::get('fetch_payload') instanceof ReentrantFetchPayload);
		var_dump(OPcache\PinnedCache::get('fetch_inner_pinned'));
	}
}

?>
--EXPECT--
volatile
unserialize-volatile
bool(true)
string(2) "ok"
pinned
unserialize-pinned
bool(true)
string(2) "ok"
