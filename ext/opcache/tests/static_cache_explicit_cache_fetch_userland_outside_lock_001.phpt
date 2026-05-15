--TEST--
OPcache explicit cache fetch materializes userland objects outside the cache read lock
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.static_cache.volatile_size_mb=32
opcache.static_cache.persistent_size_mb=32
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
			OPcache\volatile_store('fetch_inner_volatile', 'ok');
		} else {
			OPcache\persistent_store('fetch_inner_persistent', 'ok');
		}
	}
}

foreach (['volatile', 'persistent'] as $backend) {
	echo $backend, "\n";

	if ($backend === 'volatile') {
		OPcache\volatile_clear();
		OPcache\volatile_store('fetch_payload', new ReentrantFetchPayload($backend));
		var_dump(OPcache\volatile_fetch('fetch_payload') instanceof ReentrantFetchPayload);
		var_dump(OPcache\volatile_fetch('fetch_inner_volatile'));
	} else {
		OPcache\persistent_clear();
		OPcache\persistent_store('fetch_payload', new ReentrantFetchPayload($backend));
		var_dump(OPcache\persistent_fetch('fetch_payload') instanceof ReentrantFetchPayload);
		var_dump(OPcache\persistent_fetch('fetch_inner_persistent'));
	}
}

?>
--EXPECT--
volatile
unserialize-volatile
bool(true)
string(2) "ok"
persistent
unserialize-persistent
bool(true)
string(2) "ok"
