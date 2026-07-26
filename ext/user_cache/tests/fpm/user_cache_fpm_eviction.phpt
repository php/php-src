--TEST--
FPM: LRU eviction works per-partition and never wipes the pool cache
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
$cache = UserCache\Cache::getPool('p');
$blob = str_repeat('x', 8192);
$action = $_GET['action'] ?? 'fill';

if ($action === 'fill') {
    $stored = 0;
    for ($i = 0; $i < 476; $i++) {
        if (!$cache->store('k' . $i, $blob . $i)) {
            break;
        }
        $stored++;
    }
    var_dump($stored > 400);
    echo "fill\n";
    return;
}

if ($action === 'touch') {
    $alive = 0;
    for ($i = 0; $i < 10; $i++) {
        if ($cache->fetch('k' . $i) !== null) {
            $alive++;
        }
    }
    var_dump($alive === 10);
    echo "touch\n";
    return;
}

$ok = 0;
for ($i = 0; $i < 30; $i++) {
    if ($cache->store('new' . $i, $blob . 'n' . $i)) {
        $ok++;
    }
}
$touched = 0;
for ($i = 0; $i < 10; $i++) {
    if ($cache->fetch('k' . $i) !== null) {
        $touched++;
    }
}
$status = UserCache\Cache::getStatus();
var_dump($ok === 30);
var_dump($touched === 10);
var_dump($status->getEntryCount() > 400);
var_dump($status->getEvictionCount() > 0);
var_dump($status->getExpungeCount() === 0);
echo "pressure\n";
PHP;

$tester = new FPM\Tester($cfg, $code);
$tester->start(iniEntries: [
    'opcache.enable' => '1',
    'user_cache.shm_size' => '4M',
]);
$tester->expectLogStartNotices();

$tester->request(query: 'action=fill')->expectBody("bool(true)\nfill");

/* A later request re-stamps a slice of old keys with a fresh request clock. */
sleep(1);
$tester->request(query: 'action=touch')->expectBody("bool(true)\ntouch");

sleep(1);
$tester->request(query: 'action=pressure')->expectBody(
    "bool(true)\n" .
    "bool(true)\n" .
    "bool(true)\n" .
    "bool(true)\n" .
    "bool(true)\n" .
    "pressure"
);

$tester->terminate();
$tester->expectLogTerminatingNotices();
$tester->close();

?>
--EXPECT--
