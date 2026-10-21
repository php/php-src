--TEST--
UserCache\Cache: FastCGI boundary fetches of object-free values reuse one private copy per key without sharing references or stale values
--CONFLICTS--
all
--SKIPIF--
<?php
if (PHP_OS_FAMILY === 'Windows') die('skip FastCGI boundary server test is not supported on Windows');
if (!function_exists('proc_open')) die('skip proc_open() not available');
require_once __DIR__ . '/user_cache_fcgi_tester.inc';
if (FcgiTester::binary() === null) die('skip CGI SAPI binary not available');
?>
--FILE--
<?php

require_once __DIR__ . '/user_cache_fcgi_tester.inc';

$server = new FcgiTester('record-copy');

try {
    $docRoot = $server->docRoot('site', <<<'PHP'
<?php
$cache = UserCache\Cache::getPool('record-copy');
$action = $_GET['action'] ?? 'verify';

function build_list(): array
{
    $list = [];
    for ($i = 0; $i < 2000; $i++) {
        $list[] = ['id' => $i, 'name' => "item-$i"];
    }

    return $list;
}

if ($action === 'seed') {
    $ref = 1;
    $cache->store('list', build_list());
    $cache->store('refs', ['a' => &$ref, 'b' => &$ref]);
    $cache->store('object', ['o' => new ArrayObject([1])]);
    echo "seeded:ok\n";
    exit;
}

if ($action === 'warmup') {
    echo "warmup:ok\n";
    exit;
}

$out = [];

$first = $cache->fetch('list');
$out[] = $first === build_list() ? 'equal' : 'differs';

$before = memory_get_usage();
$kept = [];
for ($i = 0; $i < 100; $i++) {
    $kept[] = $cache->fetch('list');
}
$out[] = memory_get_usage() - $before < 100000 ? 'shared' : 'copied';

$first[0]['name'] = 'changed';
$first[] = 'appended';
$again = $cache->fetch('list');
$out[] = $again === build_list() ? 'isolated' : 'leaked';

$refs = $cache->fetch('refs');
$refs['a'] = 2;
$refs_again = $cache->fetch('refs');
$out[] = $refs_again['a'] === 1 && $refs_again['b'] === 1 ? 'refs-isolated' : 'refs-leaked';
$refs_again['a'] = 3;
$out[] = $refs_again['b'] === 3 ? 'refs-kept' : 'refs-broken';

$object = $cache->fetch('object');
$out[] = $cache->fetch('object')['o'] !== $object['o'] ? 'objects-fresh' : 'objects-shared';

$cache->store('list', ['replaced']);
$out[] = $cache->fetch('list') === ['replaced'] ? 'replaced' : 'stale';
$cache->store('list', build_list());

echo implode(',', $out), "\n";
PHP);
    $script = $docRoot . '/index.php';

    $server->start([
        'user_cache.enable=1',
        'user_cache.shm_size=32M',
        'opcache.file_update_protection=0',
        'display_errors=0',
    ], $script, $docRoot, 'record-copy.local', 'action=warmup');

    $server->expect('seeded:ok', $script, $docRoot, 'record-copy.local', 'action=seed');
    for ($i = 0; $i < 2; $i++) {
        echo $server->request($script, $docRoot, 'record-copy.local', 'action=verify'), "\n";
    }
} finally {
    $server->cleanup();
}

?>
--EXPECT--
equal,shared,isolated,refs-isolated,refs-kept,objects-fresh,replaced
equal,shared,isolated,refs-isolated,refs-kept,objects-fresh,replaced
