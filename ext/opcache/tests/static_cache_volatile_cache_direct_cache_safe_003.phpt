--TEST--
OPcache __DirectCacheSafe covers SPL direct paths
--EXTENSIONS--
opcache
spl
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.static_cache.volatile_size_mb=32
--FILE--
<?php

var_dump(count((new ReflectionClass(SplFixedArray::class))->getAttributes(OPcache\__DirectCacheSafe::class)));
var_dump(count((new ReflectionClass(ArrayObject::class))->getAttributes(OPcache\__DirectCacheSafe::class)));
var_dump(count((new ReflectionClass(ArrayIterator::class))->getAttributes(OPcache\__DirectCacheSafe::class)));
var_dump(count((new ReflectionClass(RecursiveArrayIterator::class))->getAttributes(OPcache\__DirectCacheSafe::class)));

class TaggedFixedArray extends SplFixedArray
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

$fixed = new TaggedFixedArray(3, 'vec', 7);
$fixed[0] = 'a';
$fixed[1] = ['nested' => 1];
$fixed[2] = 42;

var_dump(OPcache\volatile_store('safe_direct_spl_fixed', $fixed));

$fixedCopy = OPcache\volatile_fetch('safe_direct_spl_fixed');
var_dump($fixedCopy instanceof TaggedFixedArray);
var_dump($fixedCopy->getSize());
var_dump($fixedCopy[0]);
var_dump($fixedCopy[1]['nested']);
var_dump($fixedCopy[2]);
var_dump($fixedCopy->describe());

class LabelIterator extends ArrayIterator
{
}

class TaggedCollection extends ArrayObject
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

$collection = new TaggedCollection(['alpha' => 10, 'beta' => 20], 'metric', LabelIterator::class);

var_dump(OPcache\volatile_store('safe_direct_array_object', $collection));

$collectionCopy = OPcache\volatile_fetch('safe_direct_array_object');
$collectionIterator = $collectionCopy->getIterator();
var_dump($collectionCopy instanceof TaggedCollection);
var_dump($collectionIterator instanceof LabelIterator);
var_dump($collectionCopy['alpha']);
var_dump($collectionCopy['beta']);
var_dump($collectionCopy->type());

class TaggedIterator extends ArrayIterator
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

$iterator = new TaggedIterator([3, 5, 8], 'fib');

var_dump(OPcache\volatile_store('safe_direct_array_iterator', $iterator));

$iteratorCopy = OPcache\volatile_fetch('safe_direct_array_iterator');
$iteratorCopy->rewind();
var_dump($iteratorCopy instanceof TaggedIterator);
var_dump($iteratorCopy->count());
var_dump($iteratorCopy->current());
var_dump($iteratorCopy->label());

class TaggedRecursiveIterator extends RecursiveArrayIterator
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

$recursive = new TaggedRecursiveIterator(['leaf' => ['value' => 99]], 'tree');

var_dump(OPcache\volatile_store('safe_direct_recursive_array_iterator', $recursive));

$recursiveCopy = OPcache\volatile_fetch('safe_direct_recursive_array_iterator');
$recursiveCopy->rewind();
var_dump($recursiveCopy instanceof TaggedRecursiveIterator);
var_dump($recursiveCopy->count());
var_dump($recursiveCopy->hasChildren());
var_dump($recursiveCopy->name());

class CustomSerializedArrayObject extends ArrayObject
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

$custom = new CustomSerializedArrayObject(['x' => 1]);

var_dump(OPcache\volatile_store('safe_direct_array_override', $custom));

$customCopy = OPcache\volatile_fetch('safe_direct_array_override');
var_dump($customCopy instanceof CustomSerializedArrayObject);
var_dump(CustomSerializedArrayObject::$serializeCalls);
var_dump(CustomSerializedArrayObject::$unserializeCalls);
var_dump($customCopy['x']);

?>
--EXPECT--
int(1)
int(1)
int(1)
int(1)
bool(true)
bool(true)
int(3)
string(1) "a"
int(1)
int(42)
string(5) "vec:7"
bool(true)
bool(true)
bool(true)
int(10)
int(20)
string(6) "metric"
bool(true)
bool(true)
int(3)
int(3)
string(3) "fib"
bool(true)
bool(true)
int(1)
bool(true)
string(4) "tree"
bool(true)
bool(true)
int(1)
int(1)
int(1)
