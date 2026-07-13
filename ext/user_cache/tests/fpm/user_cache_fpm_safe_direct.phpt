--TEST--
FPM: UserCache\Cache safe-direct DateTime and SPL state survives requests
--EXTENSIONS--
user_cache
spl
--SKIPIF--
<?php include __DIR__ . '/skipif.inc'; ?>
--FILE--
<?php

require_once __DIR__ . '/tester.inc';

$cfg = <<<EOT
[global]
error_log = {{FILE:LOG}}
[opcache]
listen = {{ADDR}}
pm = static
pm.max_children = 1
pm.max_requests = 0
catch_workers_output = yes
EOT;

$code = <<<'PHP'
<?php
class EventDateTime extends DateTime
{
    private string $label;
    protected int $revision;

    public function __construct(string $time, DateTimeZone $timezone, string $label, int $revision)
    {
        parent::__construct($time, $timezone);
        $this->label = $label;
        $this->revision = $revision;
    }

    public function describe(): string
    {
        return $this->label . ':' . $this->revision;
    }
}

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

$cache = UserCache\Cache::getPool('default');
$action = $_GET['action'] ?? 'seed';

if ($action === 'seed') {
    $cache->clear();

    $event = new EventDateTime('2026-06-15 09:30:00.123456', new DateTimeZone('Europe/Paris'), 'launch', 7);
    $collection = new TaggedCollection(['alpha' => 10, 'beta' => 20], 'metric', LabelIterator::class);
    $fixed = SplFixedArray::fromArray(['zero', 'one', ['two']], false);
    $queue = new SplQueue();
    $queue->enqueue('q1');
    $queue->enqueue('q2');
    $priorityQueue = new SplPriorityQueue();
    $priorityQueue->setExtractFlags(SplPriorityQueue::EXTR_BOTH);
    $priorityQueue->insert('low', 1);
    $priorityQueue->insert('high', 10);

    var_dump($cache->store('safe_direct_payload', [
        'event' => $event,
        'collection' => $collection,
        'fixed' => $fixed,
        'queue' => $queue,
        'priorityQueue' => $priorityQueue,
    ]));
    echo "seed\n";
    return;
}

$payload = $cache->fetch('safe_direct_payload');
$event = $payload['event'];
$collection = $payload['collection'];
$iterator = $collection->getIterator();
$fixed = $payload['fixed'];
$queue = $payload['queue'];
$priorityQueue = $payload['priorityQueue'];

echo $event->format('Y-m-d H:i:s.u e'), ',', $event->describe(), "\n";
echo $collection->type(), ',', ($iterator instanceof LabelIterator ? 'LabelIterator' : get_debug_type($iterator)), ',', $collection['alpha'], ',', $collection['beta'], "\n";
echo $fixed->getSize(), ',', $fixed[2][0], "\n";
echo $queue->dequeue(), ',', $queue->dequeue(), "\n";
$top = $priorityQueue->extract();
echo $top['data'], ',', $top['priority'], "\n";
PHP;

$tester = new FPM\Tester($cfg, $code);
$tester->start(iniEntries: [
    'opcache.enable' => '1',
    'user_cache.shm_size' => '32M',
]);
$tester->expectLogStartNotices();

$tester->request(query: 'action=seed')->expectBody(
    "bool(true)\n" .
    "seed"
);

$tester->request(query: 'action=fetch')->expectBody(
    "2026-06-15 09:30:00.123456 Europe/Paris,launch:7\n" .
    "metric,LabelIterator,10,20\n" .
    "3,two\n" .
    "q1,q2\n" .
    "high,10"
);

$tester->terminate();
$tester->expectLogTerminatingNotices();
$tester->close();
unset($tester);
gc_collect_cycles();

echo "Done\n";

?>
--EXPECT--
Done
--CLEAN--
<?php
require_once __DIR__ . '/tester.inc';
FPM\Tester::clean();
?>
