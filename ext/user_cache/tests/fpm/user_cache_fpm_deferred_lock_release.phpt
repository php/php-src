--TEST--
FPM: a request whose entry-lock release waited for its own pins releases the lock when it ends, not on the worker's next request
--SKIPIF--
<?php
include __DIR__ . '/skipif.inc';
if (!PHP_DEBUG) die('skip requires a debug build (fault injection is ZEND_DEBUG-only)');
?>
--FILE--
<?php

require_once __DIR__ . '/tester.inc';

$cfg = <<<EOT
[global]
error_log = {{FILE:LOG}}
[www]
listen = {{ADDR}}
pm = static
pm.max_children = 2
pm.max_requests = 0
catch_workers_output = yes
EOT;

$code = <<<'PHP'
<?php
$cache = UserCache\Cache::getPool('deferred-lock-release');
$action = $_GET['action'] ?? 'check';

if ($action === 'seed') {
    $cache->clear();
    $cache->store('pinned', ['a' => 1, 'b' => [1, 2, 3]]);
    echo "seeded\n";
    return;
}

if ($action === 'hold') {
    $pinned = $cache->fetch('pinned');
    $cache->lock('job');

    for ($i = 0; $i < 500 && !$cache->has('writer-started'); $i++) {
        usleep(10000);
    }

    usleep(500000);

    $cache->unlock('job');
    echo $pinned['a'], "\n";
    return;
}

if ($action === 'crash') {
    usleep(100000);
    $cache->store('writer-started', true);
    usleep(100000);
    putenv('USER_CACHE_DEBUG_EXIT_IN_WRITE_SECTION=1');
    $cache->store('never', 'published');
    return;
}

var_dump($cache->clear(), $cache->lock('job'), $cache->unlock('job'));
PHP;

$tester = new FPM\Tester($cfg, $code);
$tester->start(iniEntries: [
    'user_cache.shm_size' => '8M',
]);
$tester->expectLogStartNotices();

$tester->request(query: 'action=seed')->expectBody('seeded');

$tester->multiRequest(
    [['query' => 'action=hold'], ['query' => 'action=crash', 'delay' => 50000]],
    errorMessage: 'the crashing worker closed its connection'
);

usleep(300000);

$tester->request(query: 'action=check')->expectBody(
    "bool(true)\n" .
    "bool(true)\n" .
    "bool(true)"
);

$tester->terminate();
$tester->close();

/* Release builds do not collect cycles at shutdown. */
unset($tester);
gc_collect_cycles();

echo "Done\n";

?>
--EXPECT--
the crashing worker closed its connection
Done
--CLEAN--
<?php
require_once __DIR__ . '/tester.inc';
FPM\Tester::clean();
?>
