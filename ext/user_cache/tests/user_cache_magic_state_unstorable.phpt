--TEST--
UserCache\Cache: magic serialization state rejects unstorable values
--EXTENSIONS--
user_cache
--INI--
user_cache.enable=1
user_cache.enable_cli=1
opcache.file_cache_only=0
user_cache.shm_size=16M
--FILE--
<?php
class UserCacheSerializeResource
{
    public $value;

    public function __serialize(): array
    {
        return ['value' => $this->value];
    }

    public function __unserialize(array $data): void
    {
        $this->value = $data['value'] ?? null;
    }
}

class UserCacheSleepResource
{
    public $value;

    public function __sleep(): array
    {
        return ['value'];
    }

    public function __wakeup(): void
    {
    }
}

function show(string $label, callable $callback): void
{
    try {
        var_dump($callback());
    } catch (TypeError $e) {
        echo $label, ": TypeError\n";
    }
}

$cache = UserCache\Cache::getPool('magic-state-unstorable');

$serializeResource = new UserCacheSerializeResource();
$serializeResource->value = fopen(__FILE__, 'r');
show('serialize-resource', fn() => $cache->store('serialize-resource', $serializeResource));
fclose($serializeResource->value);

$serializeClosure = new UserCacheSerializeResource();
$serializeClosure->value = static fn() => null;
show('serialize-closure', fn() => $cache->store('serialize-closure', $serializeClosure));

$sleepResource = new UserCacheSleepResource();
$sleepResource->value = fopen(__FILE__, 'r');
show('sleep-resource', fn() => $cache->store('sleep-resource', $sleepResource));
fclose($sleepResource->value);
?>
--EXPECT--
serialize-resource: TypeError
serialize-closure: TypeError
sleep-resource: TypeError
