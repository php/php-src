--TEST--
UserCache\Cache: same-request store()/delete() invalidate promoted request-local slots
--INI--
user_cache.enable=1
user_cache.enable_cli=1
opcache.file_cache_only=0
user_cache.shm_size=16M
--FILE--
<?php
$cache = UserCache\Cache::getPool('slot-invalidate');
$cache->clear();

class SlotInvalidateValue
{
	public static int $unserializeCalls = 0;

	public function __construct(public int $value = 0)
	{
	}

	public function __serialize(): array
	{
		return ['value' => $this->value];
	}

	public function __unserialize(array $data): void
	{
		self::$unserializeCalls++;
		$this->value = $data['value'];
	}
}

var_dump($cache->store('key', new SlotInvalidateValue(1)));
$cache->fetch('key');
$cache->fetch('key');
$promoted = $cache->fetch('key');
echo "calls after promotion: ", SlotInvalidateValue::$unserializeCalls, "\n";
echo "promoted value: ", $promoted->value, "\n";

var_dump($cache->store('key', new SlotInvalidateValue(2)));
$fresh = $cache->fetch('key');
echo "value after overwrite: ", $fresh->value, "\n";
echo "calls after overwrite fetch: ", SlotInvalidateValue::$unserializeCalls, "\n";

var_dump($cache->delete('key'));
var_dump($cache->fetch('key'));
var_dump($cache->store('key', new SlotInvalidateValue(3)));
echo "value after delete and re-store: ", $cache->fetch('key')->value, "\n";
?>
--EXPECT--
bool(true)
calls after promotion: 3
promoted value: 1
bool(true)
value after overwrite: 2
calls after overwrite fetch: 4
bool(true)
NULL
bool(true)
value after delete and re-store: 3
