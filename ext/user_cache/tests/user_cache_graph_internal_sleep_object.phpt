--TEST--
UserCache\Cache: an internal class with __sleep() is stored through the graph, restoring nested values, shared identity and hook counts as unserialize(serialize()) does, and strings keep the valid UTF-8 flag on zero-copy and private-copy fetches
--EXTENSIONS--
zend_test
--INI--
user_cache.enable=1
user_cache.enable_cli=1
--FILE--
<?php
class Plain
{
    public $value;
    protected $hidden = 'hidden';
    private $secret = 'secret';
}

class WithSerialize
{
    public array $data = [];

    public function __serialize(): array
    {
        return ['data' => $this->data];
    }

    public function __unserialize(array $data): void
    {
        $this->data = $data['data'];
    }
}

class SerializeOnly
{
    public $value = 'serialize-only';

    public function __serialize(): array
    {
        return ['value' => $this->value];
    }
}

class UnserializeOnly
{
    public $value = 'unserialize-only';
    public $seen = null;

    public function __unserialize(array $data): void
    {
        $this->value = $data['value'];
        $this->seen = array_keys($data);
    }
}

class Sleeper
{
    public static int $sleeps = 0;
    public $kept = 'kept';
    public $dropped = 'dropped';
    public $woken = false;

    public function __sleep(): array
    {
        self::$sleeps++;

        return ['kept', 'woken'];
    }

    public function __wakeup(): void
    {
        $this->woken = true;
    }
}

$cache = UserCache\Cache::getPool('graph-internal-sleep-object');

$shared = new Plain();
$shared->value = 'shared';
$reference = 'referenced';
$holes = [0 => 'a', 5 => 'b'];
unset($holes[5]);
$value = [
    'packed' => [1, 2.5, true, null, 'text'],
    'hashed' => ['a' => 1, -3 => 'negative', 'b' => ['nested' => [1, 2]]],
    'holes' => $holes,
    'empty' => [],
    'strings' => array_fill(0, 40, 'repeated string'),
    'enum' => ZendTestUnitEnum::Bar,
    'objects' => [$shared, $shared, new WithSerialize(), new SerializeOnly(), new UnserializeOnly(), new Sleeper()],
    'references' => [&$reference, &$reference],
    'inner' => new ZendTestSleepObject('inner public', 'inner protected', 'inner private'),
];
$value['objects'][2]->data = ['x' => $shared];

$object = new ZendTestSleepObject($value, 'outer protected', 'outer private');
$object->typed = 42;
$object->dynamic = 'dynamic';

$expected = serialize(unserialize(serialize($object)));
var_dump($cache->store('object', $object));

$sleeper = new Sleeper();
$outer = ['shared' => $shared, 'sleeper' => $sleeper, 'holder' => new ZendTestSleepObject(['shared' => $shared, 'sleeper' => $sleeper])];
Sleeper::$sleeps = 0;
var_dump($cache->store('outer', $outer));
var_dump(Sleeper::$sleeps);
$fetched = $cache->fetch('outer');
var_dump($fetched['holder']->public['shared'] === $fetched['shared']);
var_dump($fetched['holder']->public['sleeper'] === $fetched['sleeper']);

for ($round = 0; $round < 2; $round++) {
    $fetched = $cache->fetch('object');
    var_dump(serialize($fetched) === $expected);
    var_dump($fetched->wokenUp, $fetched->public['inner']->wokenUp, $fetched->public['objects'][5]->woken);
    var_dump($fetched->public['objects'][0] === $fetched->public['objects'][1]);
    var_dump($fetched->public['objects'][2]->data['x'] === $fetched->public['objects'][0]);
    $fetched->public['references'][0] = 'changed';
    var_dump($fetched->public['references'][1]);
}

echo "valid UTF-8 flag:\n";
$utf8 = str_repeat('é', 4000);
preg_match('//u', $utf8);
$overPinBudget = str_repeat('é', 2500000);
preg_match('//u', $overPinBudget);
var_dump(zend_test_is_string_marked_as_valid_utf8($utf8), zend_test_is_string_marked_as_valid_utf8($overPinBudget));
var_dump($cache->store('utf8', $utf8));
echo 'zero-copy root: ';
var_dump(zend_test_is_string_marked_as_valid_utf8($cache->fetch('utf8')));
var_dump($cache->store('utf8-private', $overPinBudget));
echo 'private-copy root: ';
var_dump(zend_test_is_string_marked_as_valid_utf8($cache->fetch('utf8-private')));
var_dump($cache->delete('utf8-private'));
var_dump($cache->store('utf8-private-nested', ['utf8' => $utf8, 'padding' => $overPinBudget]));
echo 'private-copy nested: ';
var_dump(zend_test_is_string_marked_as_valid_utf8($cache->fetch('utf8-private-nested')['utf8']));
?>
--EXPECT--
bool(true)
bool(true)
int(1)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
string(7) "changed"
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
string(7) "changed"
valid UTF-8 flag:
bool(true)
bool(true)
bool(true)
zero-copy root: bool(true)
bool(true)
private-copy root: bool(true)
bool(true)
bool(true)
private-copy nested: bool(true)
