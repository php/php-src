--TEST--
FPM: OPcache volatile cache persists across requests
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
$action = $_GET['action'] ?? 'seed';

if ($action === 'seed') {
    opcache_static_cache_volatile_reset();
    var_dump(OPcache\VolatileCache::getInstance('default')->store('counter', 41));
    var_dump(OPcache\VolatileCache::getInstance('default')->storeMultiple([
        'message' => 'hello from fpm',
        'payload' => ['a' => 1, 'b' => [2, 3]],
    ]));
    var_dump(OPcache\VolatileCache::getInstance('default')->store('persistent_key', 'long-lived'));
    echo "seed\n";
    return;
}

if ($action === 'fetch') {
    var_dump(OPcache\VolatileCache::getInstance('default')->fetch('counter'));
    var_dump(OPcache\VolatileCache::getInstance('default')->fetch('message'));
    var_dump(OPcache\VolatileCache::getInstance('default')->fetch('payload'));
    OPcache\VolatileCache::getInstance('default')->deleteMultiple(['message']);
    echo OPcache\VolatileCache::getInstance('default')->fetch('message', 'MISS'), "\n";
    echo "fetch\n";
    return;
}

echo OPcache\VolatileCache::getInstance('default')->fetch('persistent_key', 'MISS'), "\n";
echo "persist\n";
PHP;

$tester = new FPM\Tester($cfg, $code);
$tester->start(iniEntries: [
    'opcache.enable' => '1',
    'opcache.static_cache.volatile_size_mb' => '32',
]);
$tester->expectLogStartNotices();

$tester->request(query: 'action=seed')->expectBody(
    "bool(true)\n" .
    "bool(true)\n" .
    "bool(true)\n" .
    "seed"
);

$tester->request(query: 'action=fetch')->expectBody(
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

sleep(2);

$tester->request(query: 'action=persist')->expectBody(
    "long-lived\n" .
    "persist"
);

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
