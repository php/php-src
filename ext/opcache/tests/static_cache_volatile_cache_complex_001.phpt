--TEST--
OPcache volatile cache complex values use native encoding with lossless fallback
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.static_cache.volatile_size_mb=32
--FILE--
<?php

class SimpleUser
{
	public function __construct(
		public string $name,
		public int $age,
	) {}
}

class SerUser
{
	public static int $serializeCount = 0;
	public static int $unserializeCount = 0;

	private int $id;
	private string $name;

	public function __construct(int $id, string $name)
	{
		$this->id = $id;
		$this->name = $name;
	}

	public function __serialize(): array
	{
		self::$serializeCount++;

		return ['id' => $this->id, 'name' => $this->name];
	}

	public function __unserialize(array $data): void
	{
		self::$unserializeCount++;
		$this->id = $data['id'];
		$this->name = $data['name'];
	}

	public function info(): string
	{
		return $this->id . ':' . $this->name;
	}
}

$gap = [];
$gap[4] = 'seed';
unset($gap[4]);

$payload = [
	'nested' => ['alpha' => [1, 2, 3], 'beta' => ['x' => 'y']],
	'props' => new SimpleUser('Alice', 30),
	'serialize' => new SerUser(7, 'Bob'),
	'internal' => new DateTimeImmutable('2026-06-15 09:30:00', new DateTimeZone('UTC')),
	'gap' => $gap,
];

var_dump(OPcache\VolatileCache::set('complex', $payload));

$fetched = OPcache\VolatileCache::get('complex');
var_dump($fetched['props'] instanceof SimpleUser);
var_dump($fetched['props']->name);
var_dump($fetched['props']->age);
var_dump($fetched['serialize'] instanceof SerUser);
var_dump($fetched['serialize']->info());
var_dump(SerUser::$serializeCount);
var_dump(SerUser::$unserializeCount);
var_dump($fetched['internal'] instanceof DateTimeImmutable);
var_dump($fetched['internal']->format('Y-m-d H:i:s'));

$fetched['gap'][] = 'tail';
var_dump(array_key_last($fetched['gap']));

$shared = new stdClass();
$shared->value = 42;
var_dump(OPcache\VolatileCache::set('shared_pair', [$shared, $shared]));

$pair = OPcache\VolatileCache::get('shared_pair');
var_dump($pair[0] instanceof stdClass);
var_dump(spl_object_id($pair[0]) === spl_object_id($pair[1]));

$refs = ['value' => 1];
$refs['alias'] =& $refs['value'];
var_dump(OPcache\VolatileCache::set('refs', $refs));

$fetchedRefs = OPcache\VolatileCache::get('refs');
$fetchedRefs['alias'] = 7;
var_dump($fetchedRefs['value']);

?>
--EXPECT--
bool(true)
bool(true)
string(5) "Alice"
int(30)
bool(true)
string(5) "7:Bob"
int(1)
int(1)
bool(true)
string(19) "2026-06-15 09:30:00"
int(5)
bool(true)
bool(true)
bool(true)
bool(true)
int(7)
