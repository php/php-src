--TEST--
UserCache\Cache: class-scoped state schemas preserve magic semantics, distinguish key shapes, and restore __sleep() and __serialize() states on both sides of the shared schema size limit like unserialize()
--INI--
user_cache.enable=1
user_cache.enable_cli=1
user_cache.shm_size=16M
--FILE--
<?php
/* Shared state schema keeps per-fetch hooks, identity and class distinction */
$cache = UserCache\Cache::getPool('state-schema-hooks');

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

UserCacheSchemaPayload::$serializeCalls = 0;
UserCacheSchemaPayload::$unserializeCalls = 0;
UserCacheSchemaPayload::$events = [];
UserCacheSchemaSelfState::$selfMatches = 0;

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

/* Shape keys keep binary names, boundaries, order and class identity */
$cache = UserCache\Cache::getPool('state-schema-key-shapes');

class ShapeKeyBoundaryA
{
    public static int $stored = 0;
    public static int $restored = 0;

    public function __construct(public array $state)
    {
    }

    public function __serialize(): array
    {
        self::$stored++;
        return $this->state;
    }

    public function __unserialize(array $state): void
    {
        self::$restored++;
        $this->state = $state;
    }
}

class ShapeKeyBoundaryB extends ShapeKeyBoundaryA
{
}

ShapeKeyBoundaryA::$stored = 0;
ShapeKeyBoundaryA::$restored = 0;

$long = str_repeat('key', 512);
$states = [
    ['ab' => 1, 'c' => 2],
    ['a' => 3, 'bc' => 4],
    ['c' => 5, 'ab' => 6],
    ["a\0b" => 7, "c\0" => 8],
    [$long . 'a' => 9, $long . 'b' => 10],
    ['one' => 1, 'two' => 2, 'three' => 3, 'four' => 4,
     'five' => 5, 'six' => 6, 'seven' => 7, 'eight' => 8],
];
$payload = [];
foreach ($states as $state) {
    /* A nonnegative next-free index without a hole keeps the shaped route. */
    $state[0] = null;
    unset($state[0]);
    $payload[] = new ShapeKeyBoundaryA($state);
    $payload[] = new ShapeKeyBoundaryB($state);
}

var_dump($cache->store('payload', $payload));
for ($fetch = 0; $fetch < 2; $fetch++) {
    $fetched = $cache->fetch('payload');
    $valid = true;
    foreach ($payload as $i => $original) {
        $valid = $valid && get_class($fetched[$i]) === get_class($original)
            && $fetched[$i]->state === $original->state;
        $appended = $fetched[$i]->state;
        $appended[] = null;
        $valid = $valid && array_key_last($appended) === 1;
    }
    var_dump($valid);
    $fetched[0]->state['ab'] = -1;
}
echo ShapeKeyBoundaryA::$stored, ',', ShapeKeyBoundaryA::$restored, "\n";

class Sleepy
{
    public static int $woken = 0;

    public $skipped = 'skip';

    public function __construct(public int $id = 0, public string $name = '', public array $list = [])
    {
    }

    public function __sleep(): array
    {
        return ['id', 'name', 'list'];
    }

    public function __wakeup(): void
    {
        self::$woken++;
    }
}

class Magic
{
    public static int $restored = 0;

    public function __construct(private int $id = 0, private array $list = [])
    {
    }

    public function __serialize(): array
    {
        return ['id' => $this->id, 'list' => $this->list];
    }

    public function __unserialize(array $data): void
    {
        self::$restored++;
        $this->id = $data['id'];
        $this->list = $data['list'];
    }
}

function state_schema_limit(): void
{
    $cache = UserCache\Cache::getPool('state-schema-limit');
    /* 61 list elements make 64 state values, the most that share a schema; 62 exceed it. */
    foreach ([61, 62] as $count) {
        $sleepy = [];
        $magic = [];
        for ($i = 0; $i < 3; $i++) {
            $sleepy[] = new Sleepy($i, "sleepy-$i", range($i, $i + $count - 1));
            $magic[] = new Magic($i, array_fill(0, $count + 1, "magic-$i"));
        }
        $value = ['sleepy' => $sleepy, 'magic' => $magic];

        var_dump($cache->store("state-$count", $value));
        Sleepy::$woken = 0;
        Magic::$restored = 0;
        $fetched = $cache->fetch("state-$count");
        echo "$count: woken ", Sleepy::$woken, ', restored ', Magic::$restored, "\n";
        var_dump(serialize($fetched) === serialize(unserialize(serialize($value))));
        var_dump($fetched['sleepy'][2]->skipped, count($fetched['sleepy'][2]->list));
    }
}

echo "\nstate schema limit:\n";
state_schema_limit();
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
bool(true)
bool(true)
bool(true)
12,24

state schema limit:
bool(true)
61: woken 3, restored 3
bool(true)
string(4) "skip"
int(61)
bool(true)
62: woken 3, restored 3
bool(true)
string(4) "skip"
int(62)
