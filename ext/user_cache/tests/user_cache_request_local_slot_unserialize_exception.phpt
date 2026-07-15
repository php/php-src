--TEST--
UserCache\Cache: throwing __unserialize propagates and keeps the entry
--EXTENSIONS--
user_cache
--INI--
user_cache.enable=1
user_cache.enable_cli=1
opcache.file_cache_only=0
user_cache.shm_size=16M
--FILE--
<?php
$cache = UserCache\Cache::getPool('slot-unserialize-exception');
$cache->clear();

class SlotThrowingUnserialize
{
	public static bool $failNext = false;
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
		if (self::$failNext) {
			self::$failNext = false;
			throw new RuntimeException('unserialize failure');
		}

		$this->value = $data['value'];
	}
}

SlotThrowingUnserialize::$unserializeCalls = 0;

var_dump($cache->store('key', new SlotThrowingUnserialize(7)));

/* A throwing restore hook propagates like native unserialize(), and the entry
 * is kept because it is not corrupt. No request-local slot is seeded. */
SlotThrowingUnserialize::$failNext = true;
try {
	$cache->fetch('key');
	echo "no exception\n";
} catch (RuntimeException $e) {
	echo "caught: ", $e->getMessage(), "\n";
}
echo "calls after throwing fetch: ", SlotThrowingUnserialize::$unserializeCalls, "\n";
echo "entry kept: ";
var_dump($cache->has('key'));

/* The next fetch no longer throws and restores the value, seeding the slot. */
$restored = $cache->fetch('key');
echo "restored value: ", $restored->value, "\n";
echo "calls after successful fetch: ", SlotThrowingUnserialize::$unserializeCalls, "\n";
?>
--EXPECT--
bool(true)
caught: unserialize failure
calls after throwing fetch: 1
entry kept: bool(true)
restored value: 7
calls after successful fetch: 2
