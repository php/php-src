--TEST--
OPcache explicit cache stores handle mixed complex payloads
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.static_cache.volatile_size_mb=32
opcache.static_cache.pinned_size_mb=32
--FILE--
<?php

class ExplicitPreparedUser
{
	public function __construct(
		public string $name,
		public int $age,
	) {}
}

class ExplicitPreparedSerializableUser
{
	public static int $serializeCount = 0;
	public static int $unserializeCount = 0;

	public function __construct(
		private int $id,
		private string $name,
	) {}

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

$routePayload = [
	'nested' => ['alpha' => [1, 2, 3], 'beta' => ['x' => 'y']],
	'headers' => ['cache-control' => 'public, max-age=60'],
];

var_dump(OPcache\volatile_store_array([
	'route' => $routePayload,
	'meta' => new ExplicitPreparedUser('Alice', 30),
	'serial' => new ExplicitPreparedSerializableUser(7, 'Bob'),
	'internal' => new DateTimeImmutable('2026-06-15 09:30:00', new DateTimeZone('UTC')),
]));

$route = OPcache\volatile_fetch('route');
$meta = OPcache\volatile_fetch('meta');
$serial = OPcache\volatile_fetch('serial');
$internal = OPcache\volatile_fetch('internal');

var_dump($route['nested']['alpha'][2]);
var_dump($meta instanceof ExplicitPreparedUser);
var_dump($meta->name);
var_dump($meta->age);
var_dump($serial instanceof ExplicitPreparedSerializableUser);
var_dump($serial->info());
var_dump(ExplicitPreparedSerializableUser::$serializeCount);
var_dump(ExplicitPreparedSerializableUser::$unserializeCount);
var_dump($internal instanceof DateTimeImmutable);
var_dump($internal->format('Y-m-d H:i:s'));

OPcache\pinned_store('pinned_user', new ExplicitPreparedUser('Carol', 40));
$pinned = OPcache\pinned_fetch('pinned_user');

var_dump($pinned instanceof ExplicitPreparedUser);
var_dump($pinned->name);
var_dump($pinned->age);

var_dump(OPcache\volatile_store_array([]));

?>
--EXPECT--
bool(true)
int(3)
bool(true)
string(5) "Alice"
int(30)
bool(true)
string(5) "7:Bob"
int(1)
int(1)
bool(true)
string(19) "2026-06-15 09:30:00"
bool(true)
string(5) "Carol"
int(40)
bool(true)
