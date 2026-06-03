--TEST--
OPcache static attribute publish prepares userland-serialized values outside the cache write lock
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.static_cache.volatile_size_mb=32
opcache.static_cache.stable_size_mb=32
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
			OPcache\VolatileCache::getInstance('default')->store('publish_inner_volatile', 'ok');
		} else {
			OPcache\StableCache::getInstance('default')->store('publish_inner_stable', 'ok');
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

class StablePublishTarget
{
	#[OPcache\StableStatic]
	public static mixed $value;
}

opcache_static_cache_volatile_reset();
VolatilePublishTarget::$value = new ReentrantPublishPayload('volatile');
var_dump(OPcache\VolatileCache::getInstance('default')->fetch('publish_inner_volatile'));

OPcache\StableCache::getInstance('default')->clear();
StablePublishTarget::$value = new ReentrantPublishPayload('stable');
var_dump(OPcache\StableCache::getInstance('default')->fetch('publish_inner_stable'));

?>
--EXPECT--
serialize-volatile
string(2) "ok"
serialize-stable
string(2) "ok"
