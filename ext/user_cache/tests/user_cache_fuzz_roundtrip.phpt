--TEST--
UserCache\Cache: deterministic store/fetch round-trip fuzz cases
--EXTENSIONS--
user_cache
spl
--INI--
user_cache.enable=1
user_cache.enable_cli=1
opcache.file_cache_only=0
user_cache.shm_size=16M
date.timezone=UTC
error_reporting=E_ALL & ~E_DEPRECATED
--FILE--
<?php
$cache = UserCache\Cache::getPool('fuzz-roundtrip');

enum UCFuzzSuit: string
{
	case Hearts = 'h';
	case Spades = 's';
}

class UCFuzzLeaf
{
	public function __construct(
		public string $name,
		public int $score,
		public array $flags,
	) {
	}
}

class UCFuzzNode
{
	public ?UCFuzzNode $child = null;
	public mixed $payload = null;

	public function __construct(
		public string $name,
		public int $revision,
	) {
	}
}

class UCFuzzMagicObject
{
	public static int $serializeCalls = 0;
	public static int $unserializeCalls = 0;

	public function __construct(
		public string $name,
		public array $values,
	) {
	}

	public function __serialize(): array
	{
		self::$serializeCalls++;
		return ['name' => 'serialized-' . $this->name, 'values' => []];
	}

	public function __unserialize(array $data): void
	{
		self::$unserializeCalls++;
		$this->name = $data['name'];
		$this->values = $data['values'];
	}
}

class UCFuzzDateTime extends DateTimeImmutable
{
	public function __construct(
		string $time,
		DateTimeZone $timezone,
		public string $label,
	) {
		parent::__construct($time, $timezone);
	}
}

class UCFuzzDateModel
{
	public function __construct(
		public int $id,
		public UCFuzzDateTime $createdAt,
		public UCFuzzDateTime $updatedAt,
		public UCFuzzDateTime $publishedAt,
		public array $attributes,
	) {
	}
}

class UCFuzzArrayObject extends ArrayObject
{
	public function __construct(
		array $storage,
		public string $name,
		public int $revision,
	) {
		parent::__construct($storage, ArrayObject::ARRAY_AS_PROPS);
	}
}

function uc_fuzz_scalar(int $seed): mixed
{
	return match ($seed % 6) {
		0 => null,
		1 => $seed % 2 === 0,
		2 => $seed * 17 - 500,
		3 => $seed / 10.0,
		4 => 'str-' . $seed . '-' . str_repeat(chr(65 + ($seed % 26)), ($seed % 5) + 1),
		default => ['seed' => $seed, 'enabled' => $seed % 3 === 0],
	};
}

function uc_fuzz_array(int $seed, int $depth): array
{
	$result = [];
	$count = ($seed % 4) + 1;

	for ($i = 0; $i < $count; $i++) {
		$key = $i % 2 === 0 ? 'k' . $seed . '_' . $i : $i;
		if ($depth >= 3) {
			$result[$key] = uc_fuzz_scalar($seed + $i);
			continue;
		}

		$result[$key] = match (($seed + $i) % 5) {
			0 => uc_fuzz_scalar($seed + $i),
			1 => uc_fuzz_array($seed + $i + 1, $depth + 1),
			2 => new UCFuzzLeaf('leaf-' . $seed . '-' . $i, $seed + $i, ['hot' => $i % 2 === 0]),
			3 => new DateTimeImmutable(sprintf('2026-06-%02d 10:%02d:00.%06d', ($seed % 20) + 1, $i, $seed), new DateTimeZone('UTC')),
			default => UCFuzzSuit::Hearts,
		};
	}

	return $result;
}

function uc_fuzz_date(int $seed, string $timezone): UCFuzzDateTime
{
	return new UCFuzzDateTime(
		sprintf('2026-06-%02d %02d:%02d:%02d.%06d', ($seed % 20) + 1, $seed % 24, $seed % 60, ($seed * 3) % 60, $seed),
		new DateTimeZone($timezone),
		'label-' . $seed,
	);
}

function uc_fuzz_build_payload(int $seed): mixed
{
	switch ($seed % 16) {
		case 0:
			return uc_fuzz_scalar($seed);
		case 1:
			return uc_fuzz_array($seed, 0);
		case 2:
			return (object) [
				'name' => 'std-' . $seed,
				'items' => uc_fuzz_array($seed + 10, 0),
			];
		case 3:
			return new UCFuzzLeaf('leaf-' . $seed, $seed * 3, ['a' => true, 'b' => $seed % 2 === 0]);
		case 4:
			$shared = new UCFuzzLeaf('shared-' . $seed, $seed, ['shared' => true]);
			return ['left' => $shared, 'right' => $shared, 'list' => [$shared]];
		case 5:
			return [
				'created' => uc_fuzz_date($seed, 'Asia/Tokyo'),
				'updated' => new DateTime('2026-06-29 12:34:56.123456', new DateTimeZone('Europe/Paris')),
				'timezone' => new DateTimeZone($seed % 2 === 0 ? 'UTC' : 'America/New_York'),
				'interval' => new DateInterval('P1DT2H'),
			];
		case 6:
			return new UCFuzzArrayObject([
				'rows' => uc_fuzz_array($seed + 20, 0),
				'owner' => new UCFuzzLeaf('owner-' . $seed, $seed, []),
			], 'array-object-' . $seed, $seed);
		case 7:
			return new ArrayIterator(uc_fuzz_array($seed + 30, 0));
		case 8:
			return new RecursiveArrayIterator(['branch' => ['leaf' => uc_fuzz_array($seed + 40, 1)]]);
		case 9:
			$fixed = new SplFixedArray(3);
			$fixed[0] = 'fixed-' . $seed;
			$fixed[1] = uc_fuzz_array($seed + 50, 1);
			$fixed[2] = new UCFuzzLeaf('fixed-leaf-' . $seed, $seed, []);
			return $fixed;
		case 10:
			$list = new SplDoublyLinkedList();
			$list->setIteratorMode(SplDoublyLinkedList::IT_MODE_FIFO);
			$list->push('list-' . $seed);
			$list->push(new UCFuzzLeaf('list-leaf-' . $seed, $seed, []));
			$queue = new SplQueue();
			$queue->enqueue('queue-' . $seed);
			$queue->enqueue($seed);
			$stack = new SplStack();
			$stack->push('stack-' . $seed);
			$stack->push($seed);
			return ['list' => $list, 'queue' => $queue, 'stack' => $stack];
		case 11:
			$min = new SplMinHeap();
			$max = new SplMaxHeap();
			$pq = new SplPriorityQueue();
			$pq->setExtractFlags(SplPriorityQueue::EXTR_BOTH);
			for ($i = 0; $i < 4; $i++) {
				$min->insert($seed + $i);
				$max->insert($seed + $i);
				$pq->insert('pq-' . $seed . '-' . $i, $seed + $i);
			}
			return ['min' => $min, 'max' => $max, 'pq' => $pq];
		case 12:
			return [
				'enum' => $seed % 2 === 0 ? UCFuzzSuit::Hearts : UCFuzzSuit::Spades,
				'magic' => new UCFuzzMagicObject('magic-' . $seed, uc_fuzz_array($seed + 60, 0)),
			];
		case 13:
			return new UCFuzzDateModel(
				1000 + $seed,
				uc_fuzz_date($seed, 'UTC'),
				uc_fuzz_date($seed + 1, 'Europe/Paris'),
				uc_fuzz_date($seed + 2, 'America/Los_Angeles'),
				['status' => 'published', 'score' => $seed * 7],
			);
		case 14:
			$node = new UCFuzzNode('node-' . $seed, $seed);
			$node->payload = uc_fuzz_array($seed + 70, 0);
			$node->child = new UCFuzzNode('child-' . $seed, $seed + 1);
			$node->child->payload = new UCFuzzLeaf('nested-' . $seed, $seed, []);
			return $node;
		default:
			$root = new UCFuzzNode('cycle-root-' . $seed, $seed);
			$peer = new UCFuzzNode('cycle-peer-' . $seed, $seed + 1);
			$root->child = $peer;
			$peer->child = $root;
			$root->payload = ['peer' => $peer];
			$peer->payload = ['root' => $root];
			return $root;
	}
}

function uc_fuzz_object_properties(object $value): array
{
	$properties = [];
	foreach ((array) $value as $name => $propertyValue) {
		$properties[str_replace("\0", '\\0', (string) $name)] = $propertyValue;
	}

	ksort($properties);
	return $properties;
}

function uc_fuzz_normalize_iterable(iterable $values, SplObjectStorage $seen, int $depth): array
{
	$result = [];
	foreach ($values as $key => $value) {
		$result[(string) $key] = uc_fuzz_normalize($value, $seen, $depth + 1);
	}

	return $result;
}

function uc_fuzz_normalize_heap(SplHeap $heap, SplObjectStorage $seen, int $depth): array
{
	$copy = clone $heap;
	$values = [];

	while (!$copy->isEmpty()) {
		$values[] = uc_fuzz_normalize($copy->extract(), $seen, $depth + 1);
	}

	return $values;
}

function uc_fuzz_normalize(mixed $value, SplObjectStorage $seen, int $depth = 0): mixed
{
	if ($depth > 32) {
		return '*depth*';
	}

	if ($value === null || is_bool($value) || is_int($value) || is_float($value) || is_string($value)) {
		return $value;
	}

	if (is_array($value)) {
		$result = [];
		foreach ($value as $key => $arrayValue) {
			$result[(string) $key] = uc_fuzz_normalize($arrayValue, $seen, $depth + 1);
		}

		return ['array' => $result];
	}

	if ($value instanceof UnitEnum) {
		return [
			'enum' => get_class($value),
			'name' => $value->name,
			'value' => $value instanceof BackedEnum ? $value->value : null,
		];
	}

	if (!$value instanceof object) {
		return ['unknown' => get_debug_type($value)];
	}

	if ($seen->contains($value)) {
		return ['ref' => $seen[$value]];
	}

	$seen[$value] = $seen->count();

	if ($value instanceof DateTimeInterface) {
		return [
			'class' => get_class($value),
			'datetime' => $value->format('Y-m-d H:i:s.u P e'),
			'properties' => uc_fuzz_normalize(uc_fuzz_object_properties($value), $seen, $depth + 1),
		];
	}

	if ($value instanceof DateTimeZone) {
		return [
			'class' => get_class($value),
			'timezone' => $value->getName(),
			'properties' => uc_fuzz_normalize(uc_fuzz_object_properties($value), $seen, $depth + 1),
		];
	}

	if ($value instanceof DateInterval) {
		return [
			'class' => get_class($value),
			'interval' => $value->format('%r%y-%m-%d %h:%i:%s.%f'),
			'properties' => uc_fuzz_normalize(uc_fuzz_object_properties($value), $seen, $depth + 1),
		];
	}

	if ($value instanceof ArrayObject) {
		return [
			'class' => get_class($value),
			'flags' => $value->getFlags(),
			'iterator' => $value->getIteratorClass(),
			'storage' => uc_fuzz_normalize($value->getArrayCopy(), $seen, $depth + 1),
			'properties' => uc_fuzz_normalize(uc_fuzz_object_properties($value), $seen, $depth + 1),
		];
	}

	if ($value instanceof ArrayIterator) {
		return [
			'class' => get_class($value),
			'storage' => uc_fuzz_normalize($value->getArrayCopy(), $seen, $depth + 1),
			'properties' => uc_fuzz_normalize(uc_fuzz_object_properties($value), $seen, $depth + 1),
		];
	}

	if ($value instanceof SplFixedArray) {
		return [
			'class' => get_class($value),
			'size' => $value->getSize(),
			'storage' => uc_fuzz_normalize($value->toArray(), $seen, $depth + 1),
			'properties' => uc_fuzz_normalize(uc_fuzz_object_properties($value), $seen, $depth + 1),
		];
	}

	if ($value instanceof SplPriorityQueue) {
		return [
			'class' => get_class($value),
			'flags' => $value->getExtractFlags(),
			'values' => uc_fuzz_normalize_heap($value, $seen, $depth + 1),
			'properties' => uc_fuzz_normalize(uc_fuzz_object_properties($value), $seen, $depth + 1),
		];
	}

	if ($value instanceof SplHeap) {
		return [
			'class' => get_class($value),
			'values' => uc_fuzz_normalize_heap($value, $seen, $depth + 1),
			'properties' => uc_fuzz_normalize(uc_fuzz_object_properties($value), $seen, $depth + 1),
		];
	}

	if ($value instanceof SplDoublyLinkedList) {
		return [
			'class' => get_class($value),
			'mode' => $value->getIteratorMode(),
			'values' => uc_fuzz_normalize_iterable($value, $seen, $depth + 1),
			'properties' => uc_fuzz_normalize(uc_fuzz_object_properties($value), $seen, $depth + 1),
		];
	}

	return [
		'class' => get_class($value),
		'properties' => uc_fuzz_normalize(uc_fuzz_object_properties($value), $seen, $depth + 1),
	];
}

function uc_fuzz_digest(mixed $value): mixed
{
	return uc_fuzz_normalize($value, new SplObjectStorage());
}

$count = 128;
for ($i = 0; $i < $count; $i++) {
	$key = 'case_' . $i;
	$payload = uc_fuzz_build_payload($i);
	$before = uc_fuzz_digest($payload);

	if (!$cache->store($key, $payload)) {
		throw new RuntimeException('store failed for fuzz case ' . $i);
	}

	$missing = new stdClass();
	$fetched = $cache->fetch($key, $missing);
	if ($fetched === $missing) {
		throw new RuntimeException('fetch missed fuzz case ' . $i);
	}

	$after = uc_fuzz_digest($fetched);
	if ($before !== $after) {
		echo "mismatch case ", $i, "\n";
		var_dump($before);
		var_dump($after);
		exit(1);
	}
}

echo "fuzz cases: ", $count, "\n";
echo "magic calls: ";
var_dump([UCFuzzMagicObject::$serializeCalls, UCFuzzMagicObject::$unserializeCalls]);

unset($payload, $before, $missing, $fetched, $after);
gc_collect_cycles();
?>
--EXPECT--
fuzz cases: 128
magic calls: array(2) {
  [0]=>
  int(8)
  [1]=>
  int(8)
}
