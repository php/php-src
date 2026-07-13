--TEST--
UserCache\Cache: throwing __unserialize drops the entry and never seeds a request-local slot
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

var_dump($cache->store('key', new SlotThrowingUnserialize(7)));

/* A failed restore drops the entry without seeding the local slot. */
SlotThrowingUnserialize::$failNext = true;
var_dump($cache->fetch('key'));
echo "calls after throwing fetch: ", SlotThrowingUnserialize::$unserializeCalls, "\n";
echo "entry dropped: ";
var_dump($cache->has('key') === false);
var_dump($cache->fetch('key'));

var_dump($cache->store('key', new SlotThrowingUnserialize(8)));
$cache->fetch('key');
$cache->fetch('key');
$cloned = $cache->fetch('key');
echo "calls after re-store and three fetches: ", SlotThrowingUnserialize::$unserializeCalls, "\n";
echo "restored value: ", $cloned->value, "\n";
?>
--EXPECT--
bool(true)
NULL
calls after throwing fetch: 1
entry dropped: bool(true)
NULL
bool(true)
calls after re-store and three fetches: 4
restored value: 8
