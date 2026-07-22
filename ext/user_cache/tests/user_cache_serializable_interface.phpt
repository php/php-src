--TEST--
UserCache\Cache: legacy Serializable interface honours its serialize()/unserialize() contract
--INI--
user_cache.enable=1
user_cache.enable_cli=1
opcache.file_cache_only=0
user_cache.shm_size=16M
date.timezone=UTC
error_reporting=E_ALL & ~E_DEPRECATED
--FILE--
<?php
$cache = UserCache\Cache::getPool('serializable-interface');

function ok(string $label, bool $cond): void
{
	echo $label, ': ', $cond ? 'OK' : 'FAIL', "\n";
}

/* Serializable is deprecated, but its native handler contract must be preserved. */
class LegacyScalar implements Serializable
{
	public static int $unserializeCalls = 0;

	public function __construct(public int $value = 0)
	{
	}

	public function serialize(): string
	{
		return (string) $this->value;
	}

	public function unserialize(string $data): void
	{
		self::$unserializeCalls++;
		$this->value = (int) $data;
	}
}

$scalar = new LegacyScalar(42);
$cache->store('scalar', $scalar);
$fetched = $cache->fetch('scalar');
ok('scalar instanceof', $fetched instanceof LegacyScalar);
ok('scalar value', $fetched->value === 42);
ok('scalar unserialize ran once per fetch', LegacyScalar::$unserializeCalls === 1);
ok('scalar parity', serialize($fetched) === serialize($scalar));

class Partial implements Serializable
{
	public int $kept = 0;
	public int $notRestored = 0;

	public function serialize(): string
	{
		return (string) $this->kept;
	}

	public function unserialize(string $data): void
	{
		$this->kept = (int) $data;
	}
}
$partial = new Partial();
$partial->kept = 5;
$partial->notRestored = 99;
$cache->store('partial', $partial);
$fetchedPartial = $cache->fetch('partial');
$nativePartial = unserialize(serialize($partial));
ok('partial matches native', $fetchedPartial->kept === $nativePartial->kept
	&& $fetchedPartial->notRestored === $nativePartial->notRestored);

class LegacyBag implements Serializable
{
	public array $items = [];

	public function serialize(): string
	{
		return serialize($this->items);
	}

	public function unserialize(string $data): void
	{
		$this->items = unserialize($data);
	}
}

$bag = new LegacyBag();
$bag->items = ['a' => 1, 'b' => [2, 3], 'when' => new DateTimeImmutable('2026-01-01', new DateTimeZone('UTC'))];
$cache->store('bag', $bag);
$fetched = $cache->fetch('bag');
ok('bag instanceof', $fetched instanceof LegacyBag);
ok('bag array', $fetched->items['a'] === 1 && $fetched->items['b'] === [2, 3]);
ok('bag nested object', $fetched->items['when'] instanceof DateTimeImmutable
	&& $fetched->items['when']->format('Y-m-d') === '2026-01-01');
ok('bag parity', serialize($fetched) === serialize($bag));

$graph = ['first' => new LegacyScalar(7), 'list' => [new LegacyScalar(8), new LegacyScalar(9)]];
$cache->store('graph', $graph);
$fetched = $cache->fetch('graph');
ok('graph values', $fetched['first']->value === 7
	&& $fetched['list'][0]->value === 8
	&& $fetched['list'][1]->value === 9);
ok('graph parity', serialize($fetched) === serialize($graph));
?>
--EXPECT--
scalar instanceof: OK
scalar value: OK
scalar unserialize ran once per fetch: OK
scalar parity: OK
partial matches native: OK
bag instanceof: OK
bag array: OK
bag nested object: OK
bag parity: OK
graph values: OK
graph parity: OK
