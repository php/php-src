--TEST--
FPM: OPcache volatile and persistent caches are shared across static workers
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
pm.max_children = 4
pm.max_requests = 0
catch_workers_output = yes
EOT;

$code = <<<'PHP'
<?php
#[OPcache\PersistentStatic]
class FpmPersistentStaticWorkerShare
{
    public static int $value = 0;
}

$action = $_GET['action'] ?? 'seed';
$seedPid = (int) ($_GET['seed_pid'] ?? 0);
$pid = getmypid();
$key = 'fpm_multi_worker_volatile_cache';

if ($action === 'seed') {
    OPcache\volatile_clear();
    OPcache\volatile_store($key, 'stored-value');
    FpmPersistentStaticWorkerShare::$value = 42;
    echo 'seed:', $pid;
    return;
}

if ($pid === $seedPid) {
    usleep(200000);
    echo 'same:', $pid;
    return;
}

if ($action === 'fetch_volatile') {
    $value = OPcache\volatile_fetch($key, 'MISS');
    usleep(200000);
    echo 'fetch:', $pid, ':', $value;
    return;
}

if ($action === 'fetch_persistent') {
    $value = FpmPersistentStaticWorkerShare::$value;
    usleep(200000);
    echo 'fetch:', $pid, ':', $value;
    return;
}

echo 'unknown';
PHP;

function parseSeedPid(?string $body): int
{
    if (!preg_match('/^seed:(\d+)$/', trim((string) $body), $matches)) {
        throw new RuntimeException('Unable to parse seed response: ' . var_export($body, true));
    }
    return (int) $matches[1];
}

function expectCrossWorkerValue(FPM\Tester $tester, string $action, int $seedPid, string $expected): void
{
    $responses = $tester->multiRequest(array_fill(0, 4, [
        'query' => 'action=' . $action . '&seed_pid=' . $seedPid,
    ]), readTimeout: 7000);

    $checked = 0;
    foreach ($responses as $response) {
        $body = trim((string) $response->getBody());
        if (preg_match('/^same:\d+$/', $body)) {
            continue;
        }
        if (!preg_match('/^fetch:(\d+):(.*)$/', $body, $matches)) {
            throw new RuntimeException('Unexpected response for ' . $action . ': ' . var_export($body, true));
        }
        if ((int) $matches[1] === $seedPid) {
            throw new RuntimeException('Seed worker unexpectedly returned a fetch response for ' . $action);
        }
        if ($matches[2] !== $expected) {
            throw new RuntimeException(sprintf(
                '%s was not shared across workers: expected %s, got %s',
                $action,
                $expected,
                $matches[2]
            ));
        }
        $checked++;
    }

    if ($checked === 0) {
        throw new RuntimeException('No non-seed worker handled ' . $action);
    }
}

$tester = new FPM\Tester($cfg, $code);
$tester->start(iniEntries: [
    'opcache.enable' => '1',
    'opcache.static_cache.volatile_size_mb' => '32',
    'opcache.static_cache.persistent_size_mb' => '32',
]);
$tester->expectLogStartNotices();

$seedPid = parseSeedPid($tester->request(query: 'action=seed')->getBody());

expectCrossWorkerValue($tester, 'fetch_volatile', $seedPid, 'stored-value');
expectCrossWorkerValue($tester, 'fetch_persistent', $seedPid, '42');

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
