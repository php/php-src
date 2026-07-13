--TEST--
UserCache\Cache: __serialize() without __unserialize() restores by property assignment
--EXTENSIONS--
user_cache
--INI--
user_cache.enable=1
user_cache.enable_cli=1
opcache.file_cache_only=0
user_cache.shm_size=16M
--FILE--
<?php
$cache = UserCache\Cache::getPool('serialize-only');

function ok(string $label, bool $cond): void
{
	echo $label, ': ', $cond ? 'OK' : 'FAIL', "\n";
}

/* Match native property restoration when __unserialize() is absent. */
class SerializeOnly
{
	public int $kept = 0;
	public string $dropped = 'default';

	public function __construct(int $kept)
	{
		$this->kept = $kept;
		$this->dropped = 'set';
	}

	public function __serialize(): array
	{
		return ['kept' => $this->kept];
	}
}

$value = new SerializeOnly(5);
$cache->store('basic', $value);
$fetched = $cache->fetch('basic');
ok('basic instanceof', $fetched instanceof SerializeOnly);
ok('basic kept restored', $fetched->kept === 5);
ok('basic omitted reverts to default', $fetched->dropped === 'default');
ok('basic parity', serialize($fetched) === serialize($value));

class SerializeOnlyWakeful
{
	public static int $wakeupCalls = 0;
	public int $v = 0;

	public function __construct(int $v)
	{
		$this->v = $v;
	}

	public function __serialize(): array
	{
		return ['v' => $this->v];
	}

	public function __wakeup(): void
	{
		self::$wakeupCalls++;
	}
}

$cache->store('wakeful', new SerializeOnlyWakeful(9));
$first = $cache->fetch('wakeful');
$second = $cache->fetch('wakeful');
ok('wakeful value', $first->v === 9 && $second->v === 9);
ok('wakeful __wakeup per fetch', SerializeOnlyWakeful::$wakeupCalls === 2);
ok('wakeful distinct instances', $first !== $second);

class SerializeOnlyRich
{
	public int $num;
	private string $secret;
	public ?object $child;

	public function __construct()
	{
		$this->num = 1;
		$this->secret = 'hidden';
		$this->child = null;
	}

	public function __serialize(): array
	{
		return [
			'num' => 42,
			"\0" . self::class . "\0secret" => 'restored',
			'child' => (object) ['deep' => [1, 2, 3]],
		];
	}

	public function reveal(): string
	{
		return $this->secret;
	}
}

$rich = new SerializeOnlyRich();
$cache->store('rich', $rich);
$fetchedRich = $cache->fetch('rich');
ok('rich typed', $fetchedRich->num === 42);
ok('rich private', $fetchedRich->reveal() === 'restored');
ok('rich nested object', $fetchedRich->child instanceof stdClass && $fetchedRich->child->deep === [1, 2, 3]);
ok('rich parity', serialize($fetchedRich) === serialize($rich));

$graph = ['a' => new SerializeOnly(1), 'list' => [new SerializeOnly(2), new SerializeOnly(3)]];
$cache->store('graph', $graph);
$fetchedGraph = $cache->fetch('graph');
ok('graph values', $fetchedGraph['a']->kept === 1
	&& $fetchedGraph['list'][0]->kept === 2
	&& $fetchedGraph['list'][1]->kept === 3);
ok('graph parity', serialize($fetchedGraph) === serialize($graph));

#[\AllowDynamicProperties]
class SerializeOnlyIntKeys
{
	public int $declared = 1;

	public function __serialize(): array
	{
		return ['declared' => 7, 42 => 'answer', 'label' => 'x'];
	}
}
$intKeys = new SerializeOnlyIntKeys();
$cache->store('intkeys', $intKeys);
$fetchedIntKeys = $cache->fetch('intkeys');
ok('integer key parity', serialize($fetchedIntKeys) === serialize(unserialize(serialize($intKeys))));
ok('integer key property', $fetchedIntKeys->declared === 7 && $fetchedIntKeys->{42} === 'answer');

class SerializeOnlyResource
{
	public $handle;

	public function __serialize(): array
	{
		return ['handle' => $this->handle];
	}
}

$withResource = new SerializeOnlyResource();
$withResource->handle = fopen(__FILE__, 'r');
try {
	$cache->store('resource', $withResource);
	ok('resource rejected', false);
} catch (TypeError $e) {
	ok('resource rejected', true);
}
fclose($withResource->handle);
?>
--EXPECT--
basic instanceof: OK
basic kept restored: OK
basic omitted reverts to default: OK
basic parity: OK
wakeful value: OK
wakeful __wakeup per fetch: OK
wakeful distinct instances: OK
rich typed: OK
rich private: OK
rich nested object: OK
rich parity: OK
graph values: OK
graph parity: OK
integer key parity: OK
integer key property: OK
resource rejected: OK
