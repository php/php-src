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

#[OPcache\PinnedStatic]
class FpmPoolStatusClearResetPinnedStatic
{
    public static int $value = 0;
}

$action = $_GET['action'] ?? 'status';
$pool = $_GET['pool'] ?? 'unknown';
$volatileKey = 'fpm_pool_status_clear_reset_volatile_key';
$pinnedKey = 'fpm_pool_status_clear_reset_pinned_key';

if ($action === 'seed') {
    $base = $pool === 'alpha' ? 100 : 200;

    OPcache\volatile_store($volatileKey, $pool . '-volatile');
    OPcache\pinned_store($pinnedKey, $pool . '-pinned');
    FpmPoolStatusClearResetPinnedStatic::$value = $base + 1;
} elseif ($action === 'volatile_clear') {
    printf("volatile_clear=%d\n", OPcache\volatile_clear());
} elseif ($action === 'pinned_clear') {
    printf("pinned_clear=%d\n", OPcache\pinned_clear());
} elseif ($action === 'reset') {
    printf("reset=%d\n", opcache_reset());
}

$volatileInfo = OPcache\volatile_cache_info();
$pinnedInfo = OPcache\pinned_cache_info();
$status = opcache_get_status();
$statusVolatile = $status['volatile_cache'];
$statusPinned = $status['pinned_cache'];

printf(
    "%s:%s:%s:class=%d:info=%d/%d:status=%d/%d:eq=%d/%d\n",
    $pool,
    OPcache\volatile_fetch($volatileKey, 'MISS'),
    OPcache\pinned_fetch($pinnedKey, 'MISS'),
    FpmPoolStatusClearResetPinnedStatic::$value,
    $volatileInfo->entry_count,
    $pinnedInfo->entry_count,
    $statusVolatile->entry_count,
    $statusPinned->entry_count,
    (int) ($statusVolatile == $volatileInfo),
    (int) ($statusPinned == $pinnedInfo)
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
    'opcache.static_cache.pinned_size_mb' => '32',
    'opcache.file_update_protection' => '0',
]);
$tester->expectLogStartNotices();

$alphaSeeded = 'alpha:alpha-volatile:alpha-pinned:class=101:info=1/2:status=1/2:eq=1/1';
$betaSeeded = 'beta:beta-volatile:beta-pinned:class=201:info=1/2:status=1/2:eq=1/1';
$alphaVolatileCleared = "volatile_clear=1\nalpha:MISS:alpha-pinned:class=101:info=0/2:status=0/2:eq=1/1";
$alphaPinnedCleared = "pinned_clear=1\nalpha:alpha-volatile:MISS:class=0:info=1/0:status=1/0:eq=1/1";
$alphaReset = "reset=1\nalpha:MISS:MISS:class=0:info=0/0:status=0/0:eq=1/1";

expectPoolAction($tester, 'alpha', 'seed', $alphaSeeded);
expectPoolAction($tester, 'beta', 'seed', $betaSeeded);
expectPoolAction($tester, 'alpha', 'status', $alphaSeeded);
expectPoolAction($tester, 'beta', 'status', $betaSeeded);

expectPoolAction($tester, 'alpha', 'volatile_clear', $alphaVolatileCleared);
expectPoolAction($tester, 'beta', 'status', $betaSeeded);

expectPoolAction($tester, 'alpha', 'seed', $alphaSeeded);
expectPoolAction($tester, 'alpha', 'pinned_clear', $alphaPinnedCleared);
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
