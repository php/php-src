--TEST--
OPcache static attribute publish prepares userland-serialized values outside the cache write lock
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.static_cache.volatile_size_mb=32
opcache.static_cache.persistent_size_mb=32
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
			OPcache\volatile_store('publish_inner_volatile', 'ok');
		} else {
			OPcache\persistent_store('publish_inner_persistent', 'ok');
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

class PersistentPublishTarget
{
	#[OPcache\PersistentStatic]
	public static mixed $value;
}

OPcache\volatile_clear();
VolatilePublishTarget::$value = new ReentrantPublishPayload('volatile');
var_dump(OPcache\volatile_fetch('publish_inner_volatile'));

OPcache\persistent_clear();
PersistentPublishTarget::$value = new ReentrantPublishPayload('persistent');
var_dump(OPcache\persistent_fetch('publish_inner_persistent'));

?>
--EXPECT--
serialize-volatile
string(2) "ok"
serialize-persistent
string(2) "ok"
