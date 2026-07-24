--TEST--
UserCache\Cache: graphs with magic restore methods re-run them on every fetch
--INI--
user_cache.enable=1
user_cache.enable_cli=1
opcache.file_cache_only=0
user_cache.shm_size=16M
--FILE--
<?php
$cache = UserCache\Cache::getPool('slot-magic');
$cache->clear();

/* Restore hooks must run on every fetch. */
class SlotMagicPair
{
	public static int $serializeCalls = 0;
	public static int $unserializeCalls = 0;

	public function __construct(public int $value = 0)
	{
	}

	public function __serialize(): array
	{
		self::$serializeCalls++;
		return ['value' => $this->value];
	}

	public function __unserialize(array $data): void
	{
		self::$unserializeCalls++;
		$this->value = $data['value'];
	}
}

class SlotUnserializeOnly
{
	public static int $unserializeCalls = 0;

	public function __construct(public int $value = 0)
	{
	}

	public function __unserialize(array $data): void
	{
		self::$unserializeCalls++;
		$this->value = $data['value'];
	}
}

class SlotSleepWakeup
{
	public static int $sleepCalls = 0;
	public static int $wakeupCalls = 0;

	public function __construct(public int $value = 0)
	{
	}

	public function __sleep(): array
	{
		self::$sleepCalls++;
		return ['value'];
	}

	public function __wakeup(): void
	{
		self::$wakeupCalls++;
	}
}

var_dump($cache->store('pair', new SlotMagicPair(11)));
$first = $cache->fetch('pair');
$second = $cache->fetch('pair');
$third = $cache->fetch('pair');
$fourth = $cache->fetch('pair');
echo "pair serialize calls: ", SlotMagicPair::$serializeCalls, "\n";
echo "pair unserialize calls: ", SlotMagicPair::$unserializeCalls, "\n";
echo "pair fetches independent: ";
var_dump($third !== $fourth && $third->value === 11 && $fourth->value === 11);
$third->value = 99;
echo "pair mutation isolated: ";
var_dump($cache->fetch('pair')->value === 11);

var_dump($cache->store('unserialize-only', new SlotUnserializeOnly(22)));
$cache->fetch('unserialize-only');
$cache->fetch('unserialize-only');
$clone1 = $cache->fetch('unserialize-only');
$clone2 = $cache->fetch('unserialize-only');
echo "unserialize-only calls: ", SlotUnserializeOnly::$unserializeCalls, "\n";
echo "unserialize-only fetches independent: ";
var_dump($clone1 !== $clone2 && $clone1->value === 22 && $clone2->value === 22);

var_dump($cache->store('sleeper', new SlotSleepWakeup(33)));
$cache->fetch('sleeper');
$cache->fetch('sleeper');
$clone1 = $cache->fetch('sleeper');
$clone2 = $cache->fetch('sleeper');
echo "sleep calls: ", SlotSleepWakeup::$sleepCalls, "\n";
echo "wakeup calls: ", SlotSleepWakeup::$wakeupCalls, "\n";
echo "sleeper fetches independent: ";
var_dump($clone1 !== $clone2 && $clone1->value === 33 && $clone2->value === 33);
?>
--EXPECT--
bool(true)
pair serialize calls: 1
pair unserialize calls: 4
pair fetches independent: bool(true)
pair mutation isolated: bool(true)
bool(true)
unserialize-only calls: 4
unserialize-only fetches independent: bool(true)
bool(true)
sleep calls: 1
wakeup calls: 4
sleeper fetches independent: bool(true)
