--TEST--
OPcache static attribute publish prepares userland-serialized values outside the cache write lock
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.static_cache.volatile_size_mb=32
opcache.static_cache.pinned_size_mb=32
--FILE--
<?php

class ReentrantPublishPayload
{
	public function __construct(private string $backend)
	{
	}

	public function __serialize(): array
	{
		echo "serialize-", $this->backend, "\n";

		if ($this->backend === 'volatile') {
			OPcache\VolatileCache::set('publish_inner_volatile', 'ok');
		} else {
			OPcache\PinnedCache::set('publish_inner_pinned', 'ok');
		}

		return ['backend' => $this->backend];
	}

	public function __unserialize(array $data): void
	{
		$this->backend = $data['backend'];
	}
}

class VolatilePublishTarget
{
	#[OPcache\VolatileStatic]
	public static mixed $value;
}

class PinnedPublishTarget
{
	#[OPcache\PinnedStatic]
	public static mixed $value;
}

OPcache\VolatileCache::clear();
VolatilePublishTarget::$value = new ReentrantPublishPayload('volatile');
var_dump(OPcache\VolatileCache::get('publish_inner_volatile'));

OPcache\PinnedCache::clear();
PinnedPublishTarget::$value = new ReentrantPublishPayload('pinned');
var_dump(OPcache\PinnedCache::get('publish_inner_pinned'));

?>
--EXPECT--
serialize-volatile
string(2) "ok"
serialize-pinned
string(2) "ok"
