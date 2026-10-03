--TEST--
UserCache\Cache: objects, arrays, safe-direct DateTime, SPL and DatePeriod values persist across FPM requests
--SKIPIF--
<?php include __DIR__ . '/skipif.inc'; ?>
--FILE--
<?php

require_once __DIR__ . '/tester.inc';

$cfg = <<<EOT
[global]
error_log = {{FILE:LOG}}
[www]
listen = {{ADDR}}
pm = static
pm.max_children = 1
pm.max_requests = 0
catch_workers_output = yes
EOT;

$code = <<<'PHP'
<?php
class SimpleUser
{
    public function __construct(
        public string $name,
        public int $age,
    ) {}
}

class SerUser
{
    private int $id;
    private string $name;

    public function __construct(int $id, string $name)
    {
        $this->id = $id;
        $this->name = $name;
    }

    public function __serialize(): array
    {
        return ['id' => $this->id, 'name' => $this->name];
    }

    public function __unserialize(array $data): void
    {
        $this->id = $data['id'];
        $this->name = $data['name'];
    }

    public function info(): string
    {
        return $this->id . ':' . $this->name;
    }
}

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

class TaggedDatePeriod extends DatePeriod
{
    public string $label = 'default';

    public function tag(string $label): void
    {
        $this->label = $label;
    }
}

function period_dates(DatePeriod $period): string
{
    $dates = [];
    foreach ($period as $date) {
        $dates[] = $date->format('Y-m-d');
    }
    return implode(',', $dates);
}

[$section, $step] = explode('_', $_GET['action'], 2);
$cache = UserCache\Cache::getPool($section);

/* Object graphs, array holes, shared identity, verbatim leaves and references */
if ($section === 'complex') {
    $leaf = [str_repeat('leaf', 512), ['id' => 13]];

    if ($step === 'seed') {
        $gap = [];
        $gap[4] = 'seed';
        unset($gap[4]);

        $payload = [
            'props' => new SimpleUser('Alice', 30),
            'serialize' => new SerUser(7, 'Bob'),
            'internal' => new DateTimeImmutable('2026-06-15 09:30:00', new DateTimeZone('UTC')),
            'gap' => $gap,
        ];

        $shared = new stdClass();
        $shared->value = 42;

        $refs = ['value' => 1];
        $refs['alias'] =& $refs['value'];

        $cache->clear();
        var_dump($cache->store('complex', $payload));
        var_dump($cache->store('shared_pair', [$shared, $shared, $leaf]));
        var_dump($cache->store('refs', $refs));
        echo "seed\n";
        return;
    }

    $complex = $cache->fetch('complex');
    $complex['gap'][] = 'tail';
    echo $complex['props']->name, ',', $complex['props']->age, ',', $complex['serialize']->info(), ',', $complex['internal']->format('Y-m-d H:i:s'), ',', array_key_last($complex['gap']), "\n";

    $pair = $cache->fetch('shared_pair');
    var_dump(spl_object_id($pair[0]) === spl_object_id($pair[1]));
    var_dump($pair[2] === $leaf);
    $pair[2][1]['id'] = 99;

    $refs = $cache->fetch('refs');
    $refs['alias'] = 7;
    var_dump($refs['value']);

    echo "fetch\n";
    return;
}

/* Scalars and arrays with storeMultiple and deleteMultiple */
if ($section === 'scalars') {
    if ($step === 'seed') {
        $cache->clear();
        var_dump($cache->store('counter', 41));
        var_dump($cache->storeMultiple([
            'message' => 'hello from fpm',
            'payload' => ['a' => 1, 'b' => [2, 3]],
        ]));
        var_dump($cache->store('persistent_key', 'long-lived'));
        echo "seed\n";
        return;
    }

    if ($step === 'fetch') {
        var_dump($cache->fetch('counter'));
        var_dump($cache->fetch('message'));
        var_dump($cache->fetch('payload'));
        $cache->deleteMultiple(['message']);
        echo $cache->fetch('message', 'MISS'), "\n";
        echo "fetch\n";
        return;
    }

    echo $cache->fetch('persistent_key', 'MISS'), "\n";
    echo "persist\n";
    return;
}

/* Safe-direct DateTime subclass and SPL containers */
if ($section === 'direct') {
    if ($step === 'seed') {
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

        var_dump($cache->store('direct_payload', [
            'event' => $event,
            'collection' => $collection,
            'fixed' => $fixed,
            'queue' => $queue,
            'priorityQueue' => $priorityQueue,
        ]));
        echo "seed\n";
        return;
    }

    $payload = $cache->fetch('direct_payload');
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
    return;
}

/* Safe-direct DatePeriod by recurrences, by end date and subclassed */
if ($section === 'period') {
    if ($step === 'seed') {
        $cache->clear();

        $byRecurrences = new DatePeriod(new DateTimeImmutable('2026-01-01'), new DateInterval('P1D'), 3);
        $byEnd = new DatePeriod(
            new DateTimeImmutable('2026-03-01'),
            new DateInterval('P1M'),
            new DateTimeImmutable('2026-05-15'),
        );
        $tagged = new TaggedDatePeriod(new DateTimeImmutable('2026-02-01'), new DateInterval('P1D'), 2);
        $tagged->tag('window');

        var_dump($cache->store('period_payload', [
            'recurrences' => $byRecurrences,
            'end' => $byEnd,
            'tagged' => $tagged,
        ]));
        echo "seed\n";
        return;
    }

    $payload = $cache->fetch('period_payload');
    $recurrences = $payload['recurrences'];
    $end = $payload['end'];
    $tagged = $payload['tagged'];

    echo ($recurrences instanceof DatePeriod ? 'DatePeriod' : get_debug_type($recurrences)), ',', period_dates($recurrences), "\n";
    echo period_dates($end), "\n";
    echo ($tagged instanceof TaggedDatePeriod ? 'TaggedDatePeriod' : get_debug_type($tagged)), ',', $tagged->label, ',', period_dates($tagged), "\n";
    return;
}

throw new RuntimeException('unknown action ' . $_GET['action']);
PHP;

$tester = new FPM\Tester($cfg, $code);
$tester->start(iniEntries: [
    'user_cache.enable' => '1',
    'user_cache.shm_size' => '32M',
    'date.timezone' => 'UTC',
]);
$tester->expectLogStartNotices();

/* Object graphs, array holes, shared identity, verbatim leaves and references */
$tester->request(query: 'action=complex_seed')->expectBody(
    "bool(true)\n" .
    "bool(true)\n" .
    "bool(true)\n" .
    "seed"
);

/* Mutating the fetched leaf must not affect the following request. */
for ($i = 0; $i < 2; $i++) {
    $tester->request(query: 'action=complex_fetch')->expectBody(
        "Alice,30,7:Bob,2026-06-15 09:30:00,5\n" .
        "bool(true)\n" .
        "bool(true)\n" .
        "int(7)\n" .
        "fetch"
    );
}

/* Scalars and arrays with storeMultiple and deleteMultiple */
$tester->request(query: 'action=scalars_seed')->expectBody(
    "bool(true)\n" .
    "bool(true)\n" .
    "bool(true)\n" .
    "seed"
);

$tester->request(query: 'action=scalars_fetch')->expectBody(
    "int(41)\n" .
    "string(14) \"hello from fpm\"\n" .
    "array(2) {\n" .
    "  [\"a\"]=>\n" .
    "  int(1)\n" .
    "  [\"b\"]=>\n" .
    "  array(2) {\n" .
    "    [0]=>\n" .
    "    int(2)\n" .
    "    [1]=>\n" .
    "    int(3)\n" .
    "  }\n" .
    "}\n" .
    "MISS\n" .
    "fetch"
);

$tester->request(query: 'action=scalars_persist')->expectBody(
    "long-lived\n" .
    "persist"
);

/* Safe-direct DateTime subclass and SPL containers */
$tester->request(query: 'action=direct_seed')->expectBody(
    "bool(true)\n" .
    "seed"
);

$tester->request(query: 'action=direct_fetch')->expectBody(
    "2026-06-15 09:30:00.123456 Europe/Paris,launch:7\n" .
    "metric,LabelIterator,10,20\n" .
    "3,two\n" .
    "q1,q2\n" .
    "high,10"
);

/* Safe-direct DatePeriod by recurrences, by end date and subclassed */
$tester->request(query: 'action=period_seed')->expectBody(
    "bool(true)\n" .
    "seed"
);

$tester->request(query: 'action=period_fetch')->expectBody(
    "DatePeriod,2026-01-01,2026-01-02,2026-01-03,2026-01-04\n" .
    "2026-03-01,2026-04-01,2026-05-01\n" .
    "TaggedDatePeriod,window,2026-02-01,2026-02-02,2026-02-03"
);

$tester->terminate();
$tester->expectLogTerminatingNotices();
$tester->close();

/* Release builds do not collect cycles at shutdown. */
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
