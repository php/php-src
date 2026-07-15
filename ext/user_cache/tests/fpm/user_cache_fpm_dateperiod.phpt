--TEST--
FPM: UserCache\Cache safe-direct DatePeriod state survives across requests
--EXTENSIONS--
user_cache
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

$cache = UserCache\Cache::getPool('default');
$action = $_GET['action'] ?? 'seed';

if ($action === 'seed') {
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
PHP;

$tester = new FPM\Tester($cfg, $code);
$tester->start(iniEntries: [
    'opcache.enable' => '1',
    'user_cache.shm_size' => '32M',
    'date.timezone' => 'UTC',
]);
$tester->expectLogStartNotices();

$tester->request(query: 'action=seed')->expectBody(
    "bool(true)\n" .
    "seed"
);

$tester->request(query: 'action=fetch')->expectBody(
    "DatePeriod,2026-01-01,2026-01-02,2026-01-03,2026-01-04\n" .
    "2026-03-01,2026-04-01,2026-05-01\n" .
    "TaggedDatePeriod,window,2026-02-01,2026-02-02,2026-02-03"
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
