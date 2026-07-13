--TEST--
UserCache\Cache: class-scoped state schema and shape prototypes preserve magic semantics
--INI--
user_cache.enable=1
user_cache.enable_cli=1
opcache.file_cache_only=0
user_cache.shm_size=16M
--FILE--
<?php
class UserCacheSchemaPayload
{
	public static int $serializeCalls = 0;
	public static int $unserializeCalls = 0;
	public static array $events = [];

	public function __construct(
		private int $id,
		private string $label,
	) {
	}

	public function __serialize(): array
	{
		self::$serializeCalls++;

		return [
			'id' => $this->id,
			'label' => $this->label,
			'nested' => [
				'label' => $this->label,
				'checksum' => $this->id + strlen($this->label),
			],
		];
	}

	public function __unserialize(array $data): void
	{
		$id = $data['id'];
		$label = $data['label'];

		self::$unserializeCalls++;
		$data['id'] = -1;
		$data[] = 'local-only';
		self::$events[] = [array_keys($data), array_key_last($data)];

		$this->id = $id;
		$this->label = $label . ':' . $data['nested']['checksum'];
	}

	public function label(): string
	{
		return $this->id . ':' . $this->label;
	}
}

class UserCacheSchemaA
{
	public function __construct(private int $value)
	{
	}

	public function __serialize(): array
	{
		return ['value' => $this->value, 'tag' => 'a'];
	}

	public function __unserialize(array $data): void
	{
		$this->value = $data['value'] + 10;
	}

	public function value(): int
	{
		return $this->value;
	}
}

class UserCacheSchemaB
{
	public function __construct(private int $value)
	{
	}

	public function __serialize(): array
	{
		return ['value' => $this->value, 'tag' => 'b'];
	}

	public function __unserialize(array $data): void
	{
		$this->value = $data['value'] + 20;
	}

	public function value(): int
	{
		return $this->value;
	}
}

class UserCacheSchemaSelfState
{
	public static int $selfMatches = 0;

	public function __construct(private string $name)
	{
	}

	public function __serialize(): array
	{
		return ['self' => $this, 'name' => $this->name];
	}

	public function __unserialize(array $data): void
	{
		if ($data['self'] === $this) {
			self::$selfMatches++;
		}

		$this->name = $data['name'];
	}

	public function name(): string
	{
		return $this->name;
	}
}

$cache = UserCache\Cache::getPool('state-schema-prototype');

var_dump($cache->store('payload', [
	new UserCacheSchemaPayload(1, 'one'),
	new UserCacheSchemaPayload(2, 'two'),
	new UserCacheSchemaPayload(3, 'three'),
]));

$first = $cache->fetch('payload');
$second = $cache->fetch('payload');

echo UserCacheSchemaPayload::$serializeCalls, ',', UserCacheSchemaPayload::$unserializeCalls, "\n";
echo $first[0]->label(), '|', $first[1]->label(), '|', $first[2]->label(), "\n";
echo $second[0]->label(), '|', $second[1]->label(), '|', $second[2]->label(), "\n";
var_dump(UserCacheSchemaPayload::$events[0]);
var_dump(UserCacheSchemaPayload::$events[5]);

$sharedPayload = new UserCacheSchemaPayload(4, 'four');
var_dump($cache->store('shared-payload', [$sharedPayload, $sharedPayload]));
$sharedPayload = $cache->fetch('shared-payload');
var_dump($sharedPayload[0] === $sharedPayload[1]);
echo $sharedPayload[0]->label(), '|', $sharedPayload[1]->label(), "\n";

var_dump($cache->store('classes', [new UserCacheSchemaA(5), new UserCacheSchemaB(5)]));
$classes = $cache->fetch('classes');
echo get_class($classes[0]), ':', $classes[0]->value(), "\n";
echo get_class($classes[1]), ':', $classes[1]->value(), "\n";

var_dump($cache->store('self', new UserCacheSchemaSelfState('kept')));
$self = $cache->fetch('self');
echo $self->name(), ',', UserCacheSchemaSelfState::$selfMatches, "\n";
?>
--EXPECT--
bool(true)
3,6
1:one:4|2:two:5|3:three:8
1:one:4|2:two:5|3:three:8
array(2) {
  [0]=>
  array(4) {
    [0]=>
    string(2) "id"
    [1]=>
    string(5) "label"
    [2]=>
    string(6) "nested"
    [3]=>
    int(0)
  }
  [1]=>
  int(0)
}
array(2) {
  [0]=>
  array(4) {
    [0]=>
    string(2) "id"
    [1]=>
    string(5) "label"
    [2]=>
    string(6) "nested"
    [3]=>
    int(0)
  }
  [1]=>
  int(0)
}
bool(true)
bool(true)
4:four:8|4:four:8
bool(true)
UserCacheSchemaA:15
UserCacheSchemaB:25
bool(true)
kept,1
