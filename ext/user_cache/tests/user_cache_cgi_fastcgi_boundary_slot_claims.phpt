--TEST--
UserCache\Cache: one FastCGI process serving more boundaries than it caches slot claims for recycles reader and pin claims and keeps pinned views
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

$server = new FcgiTester('slot-claims');

try {
    $source = <<<'PHP'
<?php
$cache = UserCache\Cache::getPool('slot-claims');
$host = $_SERVER['SERVER_NAME'];
if (($_GET['action'] ?? 'fetch') === 'seed') {
    $cache->clear();
    $cache->store('graph', ['host' => $host, 'list' => range(1, 8), 'nested' => ['v' => 1.5]]);
}
$value = $cache->fetch('graph', ['host' => 'MISS']);
$status = UserCache\Cache::getStatus();
echo $host, ':', $value['host'], ':', getmypid(), ':', $status->getGraphPinnedReferences(), "\n";
PHP;
    $roots = [];
    for ($i = 1; $i <= 6; $i++) {
        $roots[$i] = $server->docRoot("site$i", $source);
    }
    $script = $roots[1] . '/index.php';

    $server->start([
        'user_cache.enable=1',
        'user_cache.shm_size=4M',
        'opcache.file_update_protection=0',
    ], $script, $roots[1], 'claims1.local', 'action=fetch');

    /* Six boundaries in one process exceed the four cached reader and pin slot claims */
    $pids = [];
    $rounds = [
        ['seed', [1, 2, 3, 4, 5, 6]],
        ['fetch', [1, 2, 5, 6, 3, 4, 1]],
    ];
    foreach ($rounds as [$action, $hosts]) {
        foreach ($hosts as $i) {
            $host = "claims$i.local";
            $line = trim($server->request($script, $roots[$i], $host, "action=$action"));
            [$served, $seen, $pid, $pinned] = explode(':', $line) + [null, null, null, null];
            if ($served !== $host || $seen !== $host || $pinned !== '1') {
                throw new RuntimeException("Unexpected response for $action $host: $line");
            }
            $pids[$pid] = true;
        }
    }

    var_dump(count($pids));
    echo "Done\n";
} finally {
    $server->cleanup();
}
?>
--EXPECT--
int(1)
Done
