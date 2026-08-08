--TEST--
UserCache\Cache: request-local slots are strictly serialize-parity-safe
--INI--
user_cache.enable=1
user_cache.enable_cli=1
opcache.file_cache_only=0
user_cache.shm_size=16M
date.timezone=UTC
--FILE--
<?php
$cache = UserCache\Cache::getPool('slot-strict-parity');
$cache->clear();

/* Restore hooks must run on every fetch; structural cloning must not run __clone(). */
class StrictParityMagicDate extends DateTime
{
	public static int $unserializeCalls = 0;
	public static int $cloneCalls = 0;

	public function __serialize(): array
	{
		return parent::__serialize();
	}

	public function __unserialize(array $data): void
	{
		self::$unserializeCalls++;
		parent::__unserialize($data);
	}

	public function __clone(): void
	{
		self::$cloneCalls++;
	}
}

/* Structural slot cloning must not invoke __clone(). */
class StrictParityPlainClone
{
	public static int $cloneCalls = 0;

	public function __construct(public int $value = 0, public array $tags = [])
	{
	}

	public function __clone(): void
	{
		self::$cloneCalls++;
	}
}

class StrictParitySleeper
{
	public static int $wakeupCalls = 0;

	public int $value = 0;

	public function __construct(int $value = 0)
	{
		$this->value = $value;
	}

	public function __sleep(): array
	{
		return ['value'];
	}

	public function __wakeup(): void
	{
		self::$wakeupCalls++;
	}
}

class StrictParityWakeupOnly
{
	public static int $wakeupCalls = 0;

	public int $value = 0;

	public function __construct(int $value = 0)
	{
		$this->value = $value;
	}

	public function __wakeup(): void
	{
		self::$wakeupCalls++;
	}
}

var_dump($cache->store('magic', new StrictParityMagicDate('2026-07-08 01:02:03.456789', new DateTimeZone('UTC'))));
$magic = null;
for ($i = 0; $i < 5; $i++) {
	$magic = $cache->fetch('magic');
}
echo "magic __unserialize calls: ", StrictParityMagicDate::$unserializeCalls, "\n";
echo "magic __clone calls: ", StrictParityMagicDate::$cloneCalls, "\n";
echo "magic round-trips: ";
var_dump($magic instanceof StrictParityMagicDate &&
	$magic->format('Y-m-d H:i:s.u') === '2026-07-08 01:02:03.456789');

var_dump($cache->store('plain', new StrictParityPlainClone(42, ['a', 'b'])));
$plainOk = true;
for ($i = 0; $i < 5; $i++) {
	$plain = $cache->fetch('plain');
	$plainOk = $plainOk && $plain->value === 42 && $plain->tags === ['a', 'b'];
}
echo "plain __clone calls: ", StrictParityPlainClone::$cloneCalls, "\n";
echo "plain round-trips: ";
var_dump($plainOk);

var_dump($cache->store('sleeper', new StrictParitySleeper(7)));
$sleeperOk = true;
for ($i = 0; $i < 5; $i++) {
	$sleeper = $cache->fetch('sleeper');
	$sleeperOk = $sleeperOk && $sleeper->value === 7;
}
echo "sleeper __wakeup calls: ", StrictParitySleeper::$wakeupCalls, "\n";
echo "sleeper round-trips: ";
var_dump($sleeperOk);

var_dump($cache->store('wakeup-only', new StrictParityWakeupOnly(9)));
$wakeupOnlyOk = true;
for ($i = 0; $i < 5; $i++) {
	$wakeupOnly = $cache->fetch('wakeup-only');
	$wakeupOnlyOk = $wakeupOnlyOk && $wakeupOnly->value === 9;
}
echo "wakeup-only __wakeup calls: ", StrictParityWakeupOnly::$wakeupCalls, "\n";
echo "wakeup-only round-trips: ";
var_dump($wakeupOnlyOk);
?>
--EXPECT--
bool(true)
magic __unserialize calls: 5
magic __clone calls: 0
magic round-trips: bool(true)
bool(true)
plain __clone calls: 0
plain round-trips: bool(true)
bool(true)
sleeper __wakeup calls: 5
sleeper round-trips: bool(true)
bool(true)
wakeup-only __wakeup calls: 5
wakeup-only round-trips: bool(true)
