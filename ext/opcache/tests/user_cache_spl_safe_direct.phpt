--TEST--
OPcache User Cache: SPL safe-direct state is restored
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_cache_only=0
opcache.user_cache_shm_size=16M
--FILE--
<?php
$cache = new Opcache\UserCache('spl-safe-direct');

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
int(0)
int(0)
bool(true)
string(8) "fallback"
int(0)
int(0)
