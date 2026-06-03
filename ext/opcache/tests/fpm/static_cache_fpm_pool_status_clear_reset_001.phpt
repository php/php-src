--TEST--
FPM: OPcache Static Cache status, clear, and reset are pool-local
--SKIPIF--
<?php include __DIR__ . '/skipif.inc'; ?>
--FILE--
<?php

require_once __DIR__ . '/tester.inc';

$cfg = <<<EOT
[global]
error_log = {{FILE:LOG}}
[alpha]
listen = {{ADDR[alpha]}}
pm = static
pm.max_children = 1
pm.max_requests = 0
catch_workers_output = yes
[beta]
listen = {{ADDR[beta]}}
pm = static
pm.max_children = 1
pm.max_requests = 0
catch_workers_output = yes
EOT;

$code = <<<'PHP'
<?php

#[OPcache\StableStatic]
class FpmPoolStatusClearResetStableStatic
{
    public static int $value = 0;
}

$action = $_GET['action'] ?? 'status';
$pool = $_GET['pool'] ?? 'unknown';
$volatileKey = 'fpm_pool_status_clear_reset_volatile_key';
$stableKey = 'fpm_pool_status_clear_reset_stable_key';

if ($action === 'seed') {
    $base = $pool === 'alpha' ? 100 : 200;

    OPcache\VolatileCache::getInstance('default')->store($volatileKey, $pool . '-volatile');
    OPcache\StableCache::getInstance('default')->store($stableKey, $pool . '-stable');
    FpmPoolStatusClearResetStableStatic::$value = $base + 1;
} elseif ($action === 'volatile_clear') {
    printf("volatile_clear=%d\n", opcache_static_cache_volatile_reset());
} elseif ($action === 'stable_clear') {
    printf("stable_clear=%d\n", OPcache\StableCache::getInstance('default')->clear());
} elseif ($action === 'reset') {
    printf("reset=%d\n", opcache_reset());
}

$volatileInfo = OPcache\VolatileCache::info();
$stableInfo = OPcache\StableCache::info();
$status = opcache_get_status();
$statusVolatile = $status['volatile_cache'];
$statusStable = $status['stable_cache'];

printf(
    "%s:%s:%s:class=%d:info=%d/%d:status=%d/%d:eq=%d/%d\n",
    $pool,
    OPcache\VolatileCache::getInstance('default')->fetch($volatileKey, 'MISS'),
    OPcache\StableCache::getInstance('default')->fetch($stableKey, 'MISS'),
    FpmPoolStatusClearResetStableStatic::$value,
    $volatileInfo->entry_count,
    $stableInfo->entry_count,
    $statusVolatile->entry_count,
    $statusStable->entry_count,
    (int) ($statusVolatile == $volatileInfo),
    (int) ($statusStable == $stableInfo)
);
PHP;

function expectPoolAction(FPM\Tester $tester, string $pool, string $action, string $expected): void
{
    $response = $tester->request(
        query: 'action=' . $action . '&pool=' . $pool,
        address: '{{ADDR[' . $pool . ']}}',
    );
    $body = trim((string) $response->getBody());
    if ($body !== $expected) {
        throw new RuntimeException(sprintf(
            "Unexpected response for pool %s action %s:\nExpected:\n%s\nGot:\n%s",
            $pool,
            $action,
            $expected,
            $body
        ));
    }
}

$tester = new FPM\Tester($cfg, $code);
$tester->start(iniEntries: [
    'opcache.enable' => '1',
    'opcache.static_cache.volatile_size_mb' => '32',
    'opcache.static_cache.stable_size_mb' => '32',
    'opcache.file_update_protection' => '0',
]);
$tester->expectLogStartNotices();

$alphaSeeded = 'alpha:alpha-volatile:alpha-stable:class=101:info=1/2:status=1/2:eq=1/1';
$betaSeeded = 'beta:beta-volatile:beta-stable:class=201:info=1/2:status=1/2:eq=1/1';
$alphaVolatileCleared = "volatile_clear=1\nalpha:MISS:alpha-stable:class=101:info=0/2:status=0/2:eq=1/1";
$alphaStableCleared = "stable_clear=1\nalpha:alpha-volatile:MISS:class=101:info=1/1:status=1/1:eq=1/1";
$alphaReset = "reset=1\nalpha:MISS:MISS:class=0:info=0/0:status=0/0:eq=1/1";

expectPoolAction($tester, 'alpha', 'seed', $alphaSeeded);
expectPoolAction($tester, 'beta', 'seed', $betaSeeded);
expectPoolAction($tester, 'alpha', 'status', $alphaSeeded);
expectPoolAction($tester, 'beta', 'status', $betaSeeded);

expectPoolAction($tester, 'alpha', 'volatile_clear', $alphaVolatileCleared);
expectPoolAction($tester, 'beta', 'status', $betaSeeded);

expectPoolAction($tester, 'alpha', 'seed', $alphaSeeded);
expectPoolAction($tester, 'alpha', 'stable_clear', $alphaStableCleared);
expectPoolAction($tester, 'beta', 'status', $betaSeeded);

expectPoolAction($tester, 'alpha', 'seed', $alphaSeeded);
expectPoolAction($tester, 'alpha', 'reset', $alphaReset);
expectPoolAction($tester, 'beta', 'status', $betaSeeded);
expectPoolAction($tester, 'alpha', 'status', 'alpha:MISS:MISS:class=0:info=0/0:status=0/0:eq=1/1');

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
