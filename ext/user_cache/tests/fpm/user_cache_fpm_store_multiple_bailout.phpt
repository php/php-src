--TEST--
FPM: UserCache\Cache storeMultiple rolls back a committed replacement when the commit loop bails out
--EXTENSIONS--
user_cache
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
[opcache]
listen = {{ADDR}}
pm = static
pm.max_children = 1
pm.max_requests = 0
catch_workers_output = yes
env[USER_CACHE_DEBUG_FORCE_BULK_COMMIT_BAILOUT] = 1
EOT;

$code = <<<'PHP'
<?php
$cache = UserCache\Cache::getPool('bulk-bailout');
$action = $_GET['action'] ?? 'seed';

if ($action === 'seed') {
    $cache->clear();
    $cache->store('a', 'ORIGINAL');
    echo UserCache\Cache::getStatus()->getFreeMemory(), "\n";
    return;
}

if ($action === 'bail') {
    /* The debug fault injection forces a bailout after 'a' commits (capturing
     * 'ORIGINAL' as its replaced entry) and before 'z' is processed. */
    $cache->storeMultiple(['a' => 'REPLACEMENT', 'z' => 'NEW']);
    echo "no-bailout\n";
    return;
}

/* The committed replacement must be rolled back: 'a' reads back as the
 * pre-batch value, 'z' was never stored, and no shared-memory block leaked
 * (free memory matches the pre-batch value printed by the seed request). */
echo $cache->fetch('a', 'MISS'), "\n";
var_dump($cache->has('z'));
echo UserCache\Cache::getStatus()->getFreeMemory(), "\n";
PHP;

$tester = new FPM\Tester($cfg, $code);
$tester->start(iniEntries: [
    'opcache.enable' => '1',
    'user_cache.shm_size' => '32M',
]);
$tester->expectLogStartNotices();

$baseline = trim($tester->request(query: 'action=seed')->getBody());

/* The bail request dies with a fatal error; the worker survives it. */
$tester->request(query: 'action=bail');

$tester->request(query: 'action=check')->expectBody(
    "ORIGINAL\n" .
    "bool(false)\n" .
    $baseline
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
