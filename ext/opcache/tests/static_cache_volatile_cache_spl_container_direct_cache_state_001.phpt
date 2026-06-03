--TEST--
OPcache volatile cache safe-direct restores SPL container state
--EXTENSIONS--
opcache
spl
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.static_cache.volatile_size_mb=32
--FILE--
<?php

opcache_static_cache_volatile_reset();

$list = new SplDoublyLinkedList();
$list->push(['score' => 11]);
$list->push((object) ['score' => 17]);

var_dump(OPcache\VolatileCache::getInstance('default')->store('safe_direct_dllist', $list));

$listFirst = OPcache\VolatileCache::getInstance('default')->fetch('safe_direct_dllist');
$listFirst[1]->score = 99;
$listSecond = OPcache\VolatileCache::getInstance('default')->fetch('safe_direct_dllist');
var_dump($listSecond instanceof SplDoublyLinkedList);
var_dump($listSecond->count());
var_dump($listSecond[0]['score']);
var_dump($listSecond[1]->score);

$queue = new SplQueue();
$queue->enqueue((object) ['score' => 23]);
$queue->enqueue((object) ['score' => 31]);

var_dump(OPcache\VolatileCache::getInstance('default')->store('safe_direct_queue', $queue));

$queueCopy = OPcache\VolatileCache::getInstance('default')->fetch('safe_direct_queue');
var_dump($queueCopy instanceof SplQueue);
var_dump($queueCopy->count());
var_dump($queueCopy[0]->score);
var_dump($queueCopy[1]->score);

$stack = new SplStack();
$stack->push((object) ['score' => 37]);
$stack->push((object) ['score' => 41]);

var_dump(OPcache\VolatileCache::getInstance('default')->store('safe_direct_stack', $stack));

$stackCopy = OPcache\VolatileCache::getInstance('default')->fetch('safe_direct_stack');
var_dump($stackCopy instanceof SplStack);
var_dump($stackCopy->count());
var_dump($stackCopy[0]->score);
var_dump($stackCopy[1]->score);

$min = new SplMinHeap();
$max = new SplMaxHeap();
for ($i = 0; $i < 4; $i++) {
	$entry = ['priority' => $i, 'node' => (object) ['score' => 100 + $i]];
	$min->insert($entry);
	$max->insert($entry);
}

var_dump(OPcache\VolatileCache::getInstance('default')->store('safe_direct_min_heap', $min));
var_dump(OPcache\VolatileCache::getInstance('default')->store('safe_direct_max_heap', $max));

$minFirst = OPcache\VolatileCache::getInstance('default')->fetch('safe_direct_min_heap');
$minFirst->top()['node']->score = 999;
$minSecond = OPcache\VolatileCache::getInstance('default')->fetch('safe_direct_min_heap');
var_dump($minSecond instanceof SplMinHeap);
var_dump($minSecond->count());
var_dump($minSecond->top()['priority']);
var_dump($minSecond->top()['node']->score);

$maxCopy = OPcache\VolatileCache::getInstance('default')->fetch('safe_direct_max_heap');
var_dump($maxCopy instanceof SplMaxHeap);
var_dump($maxCopy->count());
var_dump($maxCopy->top()['priority']);
var_dump($maxCopy->top()['node']->score);

$priorityQueue = new SplPriorityQueue();
$priorityQueue->insert((object) ['score' => 53], 5);
$priorityQueue->insert((object) ['score' => 59], 9);
$priorityQueue->setExtractFlags(SplPriorityQueue::EXTR_BOTH);

var_dump(OPcache\VolatileCache::getInstance('default')->store('safe_direct_priority_queue', $priorityQueue));

$priorityFirst = OPcache\VolatileCache::getInstance('default')->fetch('safe_direct_priority_queue');
$priorityFirst->top()['data']->score = 999;
$prioritySecond = OPcache\VolatileCache::getInstance('default')->fetch('safe_direct_priority_queue');
$priorityTop = $prioritySecond->top();
var_dump($prioritySecond instanceof SplPriorityQueue);
var_dump($prioritySecond->count());
var_dump($priorityTop['priority']);
var_dump($priorityTop['data']->score);
var_dump($prioritySecond->getExtractFlags());

class CountingMaxHeap extends SplMaxHeap
{
	public static int $compareCalls = 0;

	protected function compare(mixed $a, mixed $b): int
	{
		self::$compareCalls++;
		return $a['priority'] <=> $b['priority'];
	}
}

$counting = new CountingMaxHeap();
$counting->insert(['priority' => 1, 'node' => (object) ['score' => 61]]);
$counting->insert(['priority' => 2, 'node' => (object) ['score' => 67]]);
CountingMaxHeap::$compareCalls = 0;

var_dump(OPcache\VolatileCache::getInstance('default')->store('safe_direct_counting_heap', $counting));

$countingCopy = OPcache\VolatileCache::getInstance('default')->fetch('safe_direct_counting_heap');
var_dump($countingCopy instanceof CountingMaxHeap);
var_dump($countingCopy->top()['node']->score);
var_dump(CountingMaxHeap::$compareCalls);

class SerializedStack extends SplStack
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

$serialized = new SerializedStack();
$serialized->push('fallback');

var_dump(OPcache\VolatileCache::getInstance('default')->store('safe_direct_serialized_stack', $serialized));

$serializedCopy = OPcache\VolatileCache::getInstance('default')->fetch('safe_direct_serialized_stack');
var_dump($serializedCopy instanceof SerializedStack);
var_dump($serializedCopy[0]);
var_dump(SerializedStack::$serializeCalls);
var_dump(SerializedStack::$unserializeCalls);

?>
--EXPECT--
bool(true)
bool(true)
int(2)
int(11)
int(17)
bool(true)
bool(true)
int(2)
int(23)
int(31)
bool(true)
bool(true)
int(2)
int(41)
int(37)
bool(true)
bool(true)
bool(true)
int(4)
int(0)
int(100)
bool(true)
int(4)
int(3)
int(103)
bool(true)
bool(true)
int(2)
int(9)
int(59)
int(3)
bool(true)
bool(true)
int(67)
int(0)
bool(true)
bool(true)
string(8) "fallback"
int(1)
int(1)
