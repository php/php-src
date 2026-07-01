--TEST--
FPM: OPcache User Cache request-local lookup cache sees same-request updates
--EXTENSIONS--
opcache
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
function fetch_or_miss(Opcache\UserCache $cache, string $key): string
{
    return $cache->fetch($key, 'MISS');
}

$cache = new Opcache\UserCache('default');
$scenario = $_GET['scenario'] ?? 'hit_store';
$key = 'fpm_lookup_cache_' . $scenario . '_key';
$cache->clear();

if ($scenario === 'hit_store') {
    $cache->store($key, 'old');
    $first = fetch_or_miss($cache, $key);
    $cache->store($key, 'new');
    echo $first, "\n", fetch_or_miss($cache, $key);
    return;
}

if ($scenario === 'miss_store') {
    $first = fetch_or_miss($cache, $key);
    $cache->store($key, 'created');
    echo $first, "\n", fetch_or_miss($cache, $key);
    return;
}

if ($scenario === 'hit_delete') {
    $cache->store($key, 'old');
    $first = fetch_or_miss($cache, $key);
    $cache->delete($key);
    echo $first, "\n", fetch_or_miss($cache, $key);
    return;
}

if ($scenario === 'hit_clear') {
    $cache->store($key, 'old');
    $first = fetch_or_miss($cache, $key);
    $cache->clear();
    echo $first, "\n", fetch_or_miss($cache, $key);
    return;
}

throw new RuntimeException('unknown scenario ' . $scenario);
PHP;

$tester = new FPM\Tester($cfg, $code);
$tester->start(iniEntries: [
    'opcache.enable' => '1',
    'opcache.user_cache_shm_size' => '32M',
]);
$tester->expectLogStartNotices();

$tester->request(query: 'scenario=hit_store')->expectBody("old\nnew");
$tester->request(query: 'scenario=miss_store')->expectBody("MISS\ncreated");
$tester->request(query: 'scenario=hit_delete')->expectBody("old\nMISS");
$tester->request(query: 'scenario=hit_clear')->expectBody("old\nMISS");

$tester->terminate();
$tester->expectLogTerminatingNotices();
$tester->close();

echo "Done\n";

?>
--EXPECT--
Done
--CLEAN--
<?php
require_once __DIR__ . '/tester.inc';
FPM\Tester::clean();
?>
