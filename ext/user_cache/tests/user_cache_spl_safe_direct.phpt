--TEST--
UserCache\Cache: SPL safe-direct state and SplObjectStorage graphs are restored, and storing a corrupted heap or a heap being modified throws the same exception as serialize()
--INI--
user_cache.enable=1
user_cache.enable_cli=1
user_cache.shm_size=16M
--FILE--
<?php
/* SPL containers, subclass props, iterator classes and hook counts restore without compare() calls */
$cache = UserCache\Cache::getPool('spl-safe-direct');

class UserCacheSerializedArrayObject extends ArrayObject
{
    public static int $serializeCalls = 0;
    public static int $unserializeCalls = 0;

    public function __construct(array $data)
    {
        parent::__construct($data);
    }

    public function __serialize(): array
    {
        self::$serializeCalls++;

        return ['payload' => parent::__serialize()];
    }

    public function __unserialize(array $data): void
    {
        self::$unserializeCalls++;
        parent::__unserialize($data['payload']);
    }
}

class UserCacheSerializedStack extends SplStack
{
    public static int $serializeCalls = 0;
    public static int $unserializeCalls = 0;

    public function __serialize(): array
    {
        self::$serializeCalls++;

        return parent::__serialize();
    }

    public function __unserialize(array $data): void
    {
        self::$unserializeCalls++;
        parent::__unserialize($data);
    }
}

class UserCacheTaggedFixedArray extends SplFixedArray
{
    private string $tag;
    protected int $version;

    public function __construct(int $size, string $tag, int $version)
    {
        parent::__construct($size);
        $this->tag = $tag;
        $this->version = $version;
    }

    public function describe(): string
    {
        return $this->tag . ':' . $this->version;
    }
}

class UserCacheLabelIterator extends ArrayIterator
{
}

class UserCacheTaggedCollection extends ArrayObject
{
    private string $type;

    public function __construct(array $data, string $type, string $iteratorClass)
    {
        parent::__construct($data, 0, $iteratorClass);
        $this->type = $type;
    }

    public function type(): string
    {
        return $this->type;
    }
}

class UserCacheTaggedIterator extends ArrayIterator
{
    private string $label;

    public function __construct(array $data, string $label)
    {
        parent::__construct($data);
        $this->label = $label;
    }

    public function label(): string
    {
        return $this->label;
    }
}

class UserCacheTaggedRecursiveIterator extends RecursiveArrayIterator
{
    private string $name;

    public function __construct(array $data, string $name)
    {
        parent::__construct($data);
        $this->name = $name;
    }

    public function name(): string
    {
        return $this->name;
    }
}

class UserCacheCountingMaxHeap extends SplMaxHeap
{
    public static int $compareCalls = 0;

    protected function compare(mixed $a, mixed $b): int
    {
        self::$compareCalls++;

        return $a['priority'] <=> $b['priority'];
    }
}

$arrayObject = new ArrayObject(['a' => 1, 'b' => ['c' => 2]], ArrayObject::ARRAY_AS_PROPS);
$arrayObject->extra = 'prop';

$arrayIterator = new ArrayIterator(['x' => 10, 'y' => 20]);
$recursiveArrayIterator = new RecursiveArrayIterator(['nested' => ['leaf' => 30]]);

$fixed = SplFixedArray::fromArray(['zero', 'one', ['two']], false);

$taggedFixed = new UserCacheTaggedFixedArray(3, 'vec', 7);
$taggedFixed[0] = 'a';
$taggedFixed[1] = ['nested' => 1];
$taggedFixed[2] = 42;

$taggedCollection = new UserCacheTaggedCollection(['alpha' => 10, 'beta' => 20], 'metric', UserCacheLabelIterator::class);
$taggedIterator = new UserCacheTaggedIterator([3, 5, 8], 'fib');
$taggedRecursiveIterator = new UserCacheTaggedRecursiveIterator(['leaf' => ['value' => 99]], 'tree');

$dll = new SplDoublyLinkedList();
$dll->setIteratorMode(SplDoublyLinkedList::IT_MODE_FIFO);
$dll->push('first');
$dll->push('second');

$queue = new SplQueue();
$queue->enqueue('q1');
$queue->enqueue('q2');

$stack = new SplStack();
$stack->push('s1');
$stack->push('s2');

$min = new SplMinHeap();
$min->insert(3);
$min->insert(1);
$min->insert(2);

$max = new SplMaxHeap();
$max->insert(3);
$max->insert(1);
$max->insert(2);

$pq = new SplPriorityQueue();
$pq->setExtractFlags(SplPriorityQueue::EXTR_BOTH);
$pq->insert('low', 1);
$pq->insert('high', 10);

$countingMaxHeap = new UserCacheCountingMaxHeap();
$countingMaxHeap->insert(['priority' => 1, 'node' => (object) ['score' => 61]]);
$countingMaxHeap->insert(['priority' => 2, 'node' => (object) ['score' => 67]]);
UserCacheCountingMaxHeap::$compareCalls = 0;

$serializedArrayObject = new UserCacheSerializedArrayObject(['x' => 1]);

$serializedStack = new UserCacheSerializedStack();
$serializedStack->push('fallback');

$payload = compact(
    'arrayObject',
    'arrayIterator',
    'recursiveArrayIterator',
    'fixed',
    'taggedFixed',
    'taggedCollection',
    'taggedIterator',
    'taggedRecursiveIterator',
    'dll',
    'queue',
    'stack',
    'min',
    'max',
    'pq',
    'countingMaxHeap',
    'serializedArrayObject',
    'serializedStack'
);

var_dump($cache->store('spl', $payload));

$dirty = $cache->fetch('spl');
$dirty['arrayObject']['a'] = 999;
$clean = $cache->fetch('spl');
var_dump($clean['arrayObject']['a']);

$fetched = $cache->fetch('spl');

var_dump($fetched['arrayObject'] instanceof ArrayObject);
var_dump($fetched['arrayObject']['b']['c']);
var_dump($fetched['arrayObject']->extra);

var_dump($fetched['arrayIterator'] instanceof ArrayIterator);
var_dump(iterator_to_array($fetched['arrayIterator']));

var_dump($fetched['recursiveArrayIterator'] instanceof RecursiveArrayIterator);
var_dump($fetched['recursiveArrayIterator']->hasChildren());

var_dump($fetched['fixed'] instanceof SplFixedArray);
var_dump($fetched['fixed']->getSize());
var_dump($fetched['fixed'][2][0]);

var_dump($fetched['taggedFixed'] instanceof UserCacheTaggedFixedArray);
var_dump($fetched['taggedFixed']->getSize());
var_dump($fetched['taggedFixed'][0]);
var_dump($fetched['taggedFixed'][1]['nested']);
var_dump($fetched['taggedFixed'][2]);
var_dump($fetched['taggedFixed']->describe());

$taggedCollectionIterator = $fetched['taggedCollection']->getIterator();
var_dump($fetched['taggedCollection'] instanceof UserCacheTaggedCollection);
var_dump($taggedCollectionIterator instanceof UserCacheLabelIterator);
var_dump($fetched['taggedCollection']['alpha']);
var_dump($fetched['taggedCollection']['beta']);
var_dump($fetched['taggedCollection']->type());

$fetched['taggedIterator']->rewind();
var_dump($fetched['taggedIterator'] instanceof UserCacheTaggedIterator);
var_dump($fetched['taggedIterator']->count());
var_dump($fetched['taggedIterator']->current());
var_dump($fetched['taggedIterator']->label());

$fetched['taggedRecursiveIterator']->rewind();
var_dump($fetched['taggedRecursiveIterator'] instanceof UserCacheTaggedRecursiveIterator);
var_dump($fetched['taggedRecursiveIterator']->count());
var_dump($fetched['taggedRecursiveIterator']->hasChildren());
var_dump($fetched['taggedRecursiveIterator']->name());

var_dump($fetched['dll'] instanceof SplDoublyLinkedList);
var_dump(iterator_to_array($fetched['dll'], false));

var_dump($fetched['queue'] instanceof SplQueue);
var_dump($fetched['queue']->dequeue());
var_dump($fetched['queue']->dequeue());

var_dump($fetched['stack'] instanceof SplStack);
var_dump($fetched['stack']->pop());
var_dump($fetched['stack']->pop());

var_dump($fetched['min'] instanceof SplMinHeap);
$minOut = [];
while (!$fetched['min']->isEmpty()) {
    $minOut[] = $fetched['min']->extract();
}
var_dump($minOut);

var_dump($fetched['max'] instanceof SplMaxHeap);
$maxOut = [];
while (!$fetched['max']->isEmpty()) {
    $maxOut[] = $fetched['max']->extract();
}
var_dump($maxOut);

var_dump($fetched['pq'] instanceof SplPriorityQueue);
var_dump($fetched['pq']->extract());
var_dump($fetched['pq']->extract());

var_dump($fetched['countingMaxHeap'] instanceof UserCacheCountingMaxHeap);
var_dump($fetched['countingMaxHeap']->top()['node']->score);
var_dump(UserCacheCountingMaxHeap::$compareCalls);

var_dump($fetched['serializedArrayObject'] instanceof UserCacheSerializedArrayObject);
var_dump($fetched['serializedArrayObject']['x']);
var_dump(UserCacheSerializedArrayObject::$serializeCalls);
var_dump(UserCacheSerializedArrayObject::$unserializeCalls);

var_dump($fetched['serializedStack'] instanceof UserCacheSerializedStack);
var_dump($fetched['serializedStack'][0]);
var_dump(UserCacheSerializedStack::$serializeCalls);
var_dump(UserCacheSerializedStack::$unserializeCalls);

/* SplObjectStorage entries, info data, shared key identity, subclasses and nesting round-trip */
function ok(string $label, bool $cond): void
{
    echo $label, ': ', $cond ? 'OK' : 'FAIL', "\n";
}

class TaggedStorage extends SplObjectStorage
{
    public string $label = 'default';
}

$cache = UserCache\Cache::getPool('spl-storage-graph');

$a = new stdClass();
$a->id = 1;
$b = new stdClass();
$b->id = 2;
$c = new stdClass();
$c->id = 3;

$storage = new SplObjectStorage();
$storage[$a] = 'scalar-data';
$storage[$b] = ['nested' => [1, 2, 3], 'flag' => true];
$storage[$c] = (object) ['tag' => 'object-data'];

$cache->store('storage', $storage);
$fetched = $cache->fetch('storage');

ok('instanceof', $fetched instanceof SplObjectStorage);
ok('count', count($fetched) === 3);

$byId = [];
foreach ($fetched as $object) {
    $byId[$object->id] = $fetched->getInfo();
}
ok('data preserved', $byId[1] === 'scalar-data'
    && $byId[2] === ['nested' => [1, 2, 3], 'flag' => true]
    && $byId[3] instanceof stdClass && $byId[3]->tag === 'object-data');
ok('parity', serialize($fetched) === serialize($storage));

$shared = new stdClass();
$shared->id = 100;
$sharedStorage = new SplObjectStorage();
$sharedStorage[$shared] = 'info';
$graph = ['storage' => $sharedStorage, 'also' => $shared];

$cache->store('graph', $graph);
$fetchedGraph = $cache->fetch('graph');
$fetchedGraph['storage']->rewind();
$keyObject = $fetchedGraph['storage']->current();
ok('shared key identity', $keyObject === $fetchedGraph['also']);
$fetchedGraph['also']->id = 200;
ok('shared mutation follows', $keyObject->id === 200);

$empty = new SplObjectStorage();
$cache->store('empty', $empty);
ok('empty storage', count($cache->fetch('empty')) === 0);

$tagged = new TaggedStorage();
$tagged->label = 'tagged';
$element = new stdClass();
$element->id = 5;
$tagged[$element] = 'x';
$cache->store('tagged', $tagged);
$fetchedTagged = $cache->fetch('tagged');
ok('subclass instanceof', $fetchedTagged instanceof TaggedStorage);
ok('subclass property', $fetchedTagged->label === 'tagged' && count($fetchedTagged) === 1);
ok('subclass parity', serialize($fetchedTagged) === serialize($tagged));

$inner = new SplObjectStorage();
$innerKey = new stdClass();
$innerKey->id = 7;
$inner[$innerKey] = 'inner';
$outer = new SplObjectStorage();
$outerKey = new stdClass();
$outerKey->id = 8;
$outer[$outerKey] = $inner;
$cache->store('nested', $outer);
$fetchedNested = $cache->fetch('nested');
$fetchedNested->rewind();
$nestedInfo = $fetchedNested->getInfo();
ok('nested storage', $nestedInfo instanceof SplObjectStorage && count($nestedInfo) === 1);
ok('nested parity', serialize($fetchedNested) === serialize($outer));

class ThrowingHeap extends SplMinHeap
{
    public bool $throw = false;

    protected function compare($value1, $value2): int
    {
        if ($this->throw) {
            throw new Exception('compare failed');
        }

        return parent::compare($value1, $value2);
    }
}

class StoringQueue extends SplPriorityQueue
{
    public ?Closure $hook = null;

    public function compare($priority1, $priority2): int
    {
        if ($this->hook !== null) {
            $hook = $this->hook;
            $this->hook = null;
            $hook($this);
        }

        return parent::compare($priority1, $priority2);
    }
}

function show(string $label, Closure $callback): void
{
    try {
        var_dump($callback());
    } catch (Throwable $e) {
        echo $label, ': ', $e::class, ': ', $e->getMessage(), "\n";
    }
}

echo "\nspl heap unstorable state:\n";
$cache = UserCache\Cache::getPool('spl-heap-unstorable-state');

$heap = new ThrowingHeap;
$heap->insert(1);
$heap->insert(2);
$heap->throw = true;
try {
    $heap->insert(3);
} catch (Exception $e) {
}

show('serialize', static fn () => serialize($heap));
show('store', static fn () => $cache->store('corrupted', $heap));
var_dump($cache->has('corrupted'));

$queue = new StoringQueue;
$queue->insert('a', 1);
$queue->hook = static function (StoringQueue $queue): void {
    show('serialize', static fn () => serialize($queue));
    show('store', static fn () => $GLOBALS['cache']->store('modified', $queue));
};
$queue->insert('b', 2);
var_dump($cache->has('modified'));
?>
--EXPECT--
bool(true)
int(1)
bool(true)
int(2)
string(4) "prop"
bool(true)
array(2) {
  ["x"]=>
  int(10)
  ["y"]=>
  int(20)
}
bool(true)
bool(true)
bool(true)
int(3)
string(3) "two"
bool(true)
int(3)
string(1) "a"
int(1)
int(42)
string(5) "vec:7"
bool(true)
bool(true)
int(10)
int(20)
string(6) "metric"
bool(true)
int(3)
int(3)
string(3) "fib"
bool(true)
int(1)
bool(true)
string(4) "tree"
bool(true)
array(2) {
  [0]=>
  string(5) "first"
  [1]=>
  string(6) "second"
}
bool(true)
string(2) "q1"
string(2) "q2"
bool(true)
string(2) "s2"
string(2) "s1"
bool(true)
array(3) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
}
bool(true)
array(3) {
  [0]=>
  int(3)
  [1]=>
  int(2)
  [2]=>
  int(1)
}
bool(true)
array(2) {
  ["data"]=>
  string(4) "high"
  ["priority"]=>
  int(10)
}
array(2) {
  ["data"]=>
  string(3) "low"
  ["priority"]=>
  int(1)
}
bool(true)
int(67)
int(0)
bool(true)
int(1)
int(1)
int(3)
bool(true)
string(8) "fallback"
int(1)
int(3)
instanceof: OK
count: OK
data preserved: OK
parity: OK
shared key identity: OK
shared mutation follows: OK
empty storage: OK
subclass instanceof: OK
subclass property: OK
subclass parity: OK
nested storage: OK
nested parity: OK

spl heap unstorable state:
serialize: RuntimeException: Heap is corrupted, heap properties are no longer ensured.
store: RuntimeException: Heap is corrupted, heap properties are no longer ensured.
bool(false)
serialize: RuntimeException: Cannot serialize heap while it is being modified.
store: RuntimeException: Cannot serialize heap while it is being modified.
bool(false)
